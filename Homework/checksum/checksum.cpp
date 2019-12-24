#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

uint16_t be16(const uint8_t* a) {
    return ((uint16_t)a[0] << 8) | a[1];
}
uint16_t checksum(uint8_t* packet, size_t len) {
    uint32_t sum = 0;
    len >>= 1;
    for (int i = 0; i < 5; ++i) {
        sum += be16(packet);
        packet += 2;
    }
    packet += 2;
    for (int i = 6; i < len; ++i) {
        sum += be16(packet);
        packet += 2;
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}
/**
 * @brief 进行 IP 头的校验和的验证
 * @param packet 完整的 IP 头和载荷
 * @param len 即 packet 的长度，单位是字节，保证包含完整的 IP 头
 * @return 校验和无误则返回 true ，有误则返回 false
 */
bool validateIPChecksum(uint8_t *packet, size_t packet_len) {
  // TODO:
  //size_t head_len = *packet >> 4;
  size_t head_len = (*packet & 0xf) * 4;
  if (head_len > packet_len) {
      return false;
  }

  return checksum(packet, head_len) == be16(packet + 10);
}
