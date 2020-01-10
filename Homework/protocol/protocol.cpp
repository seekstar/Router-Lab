#include "rip.h"
#include <stdint.h>
#include <stdlib.h>
#include <endian.h>

#include "myendian.h"
#include "myip.h"

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#endif

int lowbit(int x) {
	return x & -x;
}
bool test_single_one(int x) {
	return lowbit(x) == x;
}
//if x==0 return true
bool test_01(int x) {
	return test_single_one(x + 1);
}
bool test_10(int x) {
  return test_01(~x);
}

bool valid_metric(uint32_t metric) {
  return 1 <= metric && metric <= 16;
}

/*
  在头文件 rip.h 中定义了如下的结构体：
  #define RIP_MAX_ENTRY 25
  typedef struct {
    // all fields are big endian
    // we don't store 'family', as it is always 2(for response) and 0(for request)
    // we don't store 'tag', as it is always 0
    uint32_t addr;
    uint32_t mask;
    uint32_t nexthop;
    uint32_t metric;
  } RipEntry;

  typedef struct {
    uint32_t numEntries;
    // all fields below are big endian
    uint8_t command; // 1 for request, 2 for response, otherwsie invalid
    // we don't store 'version', as it is always 2
    // we don't store 'zero', as it is always 0
    RipEntry entries[RIP_MAX_ENTRY];
  } RipPacket;

  你需要从 IPv4 包中解析出 RipPacket 结构体，也要从 RipPacket 结构体构造出对应的 IP 包
  由于 Rip 包结构本身不记录表项的个数，需要从 IP 头的长度中推断，所以在 RipPacket 中额外记录了个数。
  需要注意这里的地址都是用 **大端序** 存储的，1.2.3.4 对应 0x04030201 。
*/

/**
 * @brief 从接受到的 IP 包解析出 Rip 协议的数据
 * @param packet 接受到的 IP 包
 * @param len 即 packet 的长度
 * @param output 把解析结果写入 *output
 * @return 如果输入是一个合法的 RIP 包，把它的内容写入 RipPacket 并且返回 true；否则返回 false
 * 
 * IP 包的 Total Length 长度可能和 len 不同，当 Total Length 大于 len 时，把传入的 IP 包视为不合法。
 * 你不需要校验 IP 头和 UDP 的校验和是否合法。
 * 你需要检查 Command 是否为 1 或 2，Version 是否为 2， Zero 是否为 0，
 * Family 和 Command 是否有正确的对应关系（见上面结构体注释），Tag 是否为 0，
 * Metric 转换成小端序后是否在 [1,16] 的区间内，
 * Mask 的二进制是不是连续的 1 与连续的 0 组成等等。
 */
bool disassemble(const uint8_t *packet, uint32_t len, RipPacket *output) {
  // DONE:
  if (rbe16(packet + 2) > len) {
    //Total length > len
#if DEBUG
    printf("Total length > len\n");
#endif
    return false;
  }
  uint32_t ip_head_length = ip_head_len(packet);
  if (!validateUDPChecksum(packet + ip_head_length)) {
#if DEBUG
    printf("UDP checksum wrong\n");
#endif
    return false;
  }
  output->numEntries = (len - (ip_head_length + 8 + 2)) / 20;
  output->command = packet[ip_head_length + 8];
  if ((output->command != 1 && output->command != 2) || 
      packet[ip_head_length + 9] != 2 || //Version != RIPv2
      (packet[ip_head_length + 10] || packet[ip_head_length + 11])  //Zero != 0
  ) {
#if DEBUG
    printf("rip head incorrect, command = %d, version = %d, zero1 = %d, zero2 = %d\n", output->command, packet[ip_head_length + 9], packet[ip_head_length + 10], packet[ip_head_length + 11]);
#endif
    return false;
  }
  packet += ip_head_length + 8 + 4;
  uint16_t family = output->command == 2 ? 2 : 0; //0 for request, 2 for response.
  for (uint32_t i = 0; i < output->numEntries; ++i, packet += 20) {
    if (rbe16(packet) != family ||
        rbe16(packet + 2) != 0 ||  //tag != 0
        !test_10(rbe32(packet + 8)) ||  //Mask is invalid
        !valid_metric(rbe32(packet + 16))  //metric is not in [1, 16]
    ) {
#if DEBUG
      printf("%d-th incorrect\n", i);
      printf("family = %d, tag = %d, mask = %p, metric = %d\n", rbe16(packet), rbe16(packet + 2), rbe32(packet + 8), rbe16(packet + 16));
      printf("std family = %d\n", family);
#endif
      return false;
    }
    output->entries[i] = {
      //all big endial
      .addr = *(uint32_t*)(packet + 4),
      .mask = *(uint32_t*)(packet + 8),
      .nexthop = *(uint32_t*)(packet + 12),
      .metric = *(uint32_t*)(packet + 16)
    };
  }
  return true;
}

void assemble(uint8_t* buffer, const RipEntry& rip, uint16_t family) {
  wbe16(buffer, family);
  buffer += 2;
  *(uint16_t*)buffer = 0;
  buffer += 2;
  *(uint32_t*)buffer = rip.addr;
  buffer += 4;
  *(uint32_t*)buffer = rip.mask;
  buffer += 4;
  *(uint32_t*)buffer = rip.nexthop;
  buffer += 4;
  *(uint32_t*)buffer = rip.metric;
}

/**
 * @brief 从 RipPacket 的数据结构构造出 RIP 协议的二进制格式
 * @param rip 一个 RipPacket 结构体
 * @param buffer 一个足够大的缓冲区，你要把 RIP 协议的数据写进去
 * @return 写入 buffer 的数据长度
 * 
 * 在构造二进制格式的时候，你需要把 RipPacket 中没有保存的一些固定值补充上，包括 Version、Zero、Address Family 和 Route Tag 这四个字段
 * 你写入 buffer 的数据长度和返回值都应该是四个字节的 RIP 头，加上每项 20 字节。
 * 需要注意一些没有保存在 RipPacket 结构体内的数据的填写。
 */
uint32_t assemble(const RipPacket *rip, uint8_t *buffer) {
  // TODO:
  *(buffer++) = rip->command;
  uint16_t family = rip->command == 2 ? 2 : 0; //0 for request, 2 for response.
  *(buffer++) = 2;
  *(uint16_t*)buffer = 0;
  buffer += 2;
  for (uint32_t i = 0; i < rip->numEntries; ++i, buffer += 20) {
    assemble(buffer, rip->entries[i], family);
  }

  return 4 + 20 * rip->numEntries;
}
