#include "checksum.h"
#include "myendian.h"
#include "myip.h"
#include "endian.h"

uint16_t checksum(const uint8_t* packet, size_t head_len, size_t checksum_pos) {
    uint32_t sum = 0;
    size_t i;
    for (i = 0; i < checksum_pos; i += 2) {
        sum += *(uint16_t*)(packet + i);
    }
    for (i = checksum_pos + 2; i < head_len; i += 2) {
        sum += *(uint16_t*)(packet + i);
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}

/*// host endian
uint16_t checksum_ip_he(uint8_t* packet, size_t head_len) {
    return checksum_he(packet, head_len, 10);
}
// host endian
uint16_t checksum_ip_he(uint8_t* packet) {
    return checksum_ip_he(packet, ip_head_len(packet));
}*/

uint16_t checksum_ip(const uint8_t* packet, size_t head_len) {
    return checksum(packet, head_len, 10);
}
uint16_t checksum_ip(const uint8_t* packet) {
    return checksum_ip(packet, ip_head_len(packet));
}
void fill_ip_checksum(uint8_t* ip) {
    *(uint16_t*)(ip + 10) = checksum_ip(ip);
}


/**
 * @brief 进行 IP 头的校验和的验证
 * @param packet 完整的 IP 头和载荷
 * @param len 即 packet 的长度，单位是字节，保证包含完整的 IP 头
 * @return 校验和无误则返回 true ，有误则返回 false
 */
bool validateIPChecksum(const uint8_t *packet, size_t packet_len) {
  // DONE:
  size_t head_len = ip_head_len(packet);
  if (head_len > packet_len) {
      return false;
  }

  return checksum_ip(packet, head_len) == *(uint16_t*)(packet + 10);
}

// big endian
uint16_t checksum_udp(const uint8_t* packet) {
    //return checksum(packet, 8, 6);
    return 0;
}
void fill_udp_checksum(uint8_t* packet) {
    *(uint16_t*)(packet + 6) = checksum_udp(packet);
}
bool validateUDPChecksum(const uint8_t* packet) {
    return checksum_udp(packet) == *(uint16_t*)(packet + 6);
}
