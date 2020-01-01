#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include <stdint.h>
#include <stdlib.h>

uint16_t checksum_he(uint8_t* packet, size_t head_len, size_t checksum_pos);

uint16_t checksum_be(uint8_t* packet, size_t head_len, size_t checksum_pos);
uint16_t checksum_ip_be(uint8_t* packet, size_t head_len);
uint16_t checksum_ip_be(uint8_t* packet);
void fill_ip_checksum(uint8_t* ip);

bool validateIPChecksum(uint8_t *packet, size_t len);

uint16_t checksum_udp_be(uint8_t* packet);
void fill_udp_checksum(uint8_t* packet);

#endif
