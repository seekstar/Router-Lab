#include "checksum.h"
#include "myendian.h"
#include "myip.h"

uint16_t checksum(uint8_t* packet, size_t head_len, size_t checksum_pos) {
    uint32_t sum = 0;
    size_t i;
    for (i = 0; i < checksum_pos; i += 2) {
        sum += rbe16(packet + i);
    }
    for (i = checksum_pos + 2; i < head_len; i += 2) {
        sum += rbe16(packet + i);
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}

uint16_t checksum(uint8_t* packet, size_t head_len) {
    return checksum(packet, head_len, 10);
}
uint16_t checksum(uint8_t* packet) {
    return checksum(packet, ip_head_len(packet));
}

/**
 * @brief 进行 IP 头的校验和的验证
 * @param packet 完整的 IP 头和载荷
 * @param len 即 packet 的长度，单位是字节，保证包含完整的 IP 头
 * @return 校验和无误则返回 true ，有误则返回 false
 */
bool validateIPChecksum(uint8_t *packet, size_t packet_len) {
  // TODO:
  size_t head_len = ip_head_len(packet);
  if (head_len > packet_len) {
      return false;
  }

  return checksum(packet, head_len) == rbe16(packet + 10);
}

uint16_t checksum_udp(uint8_t* packet) {
    return checksum(packet, 8, 6);
}
