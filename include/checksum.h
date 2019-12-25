#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include <stdint.h>
#include <stdlib.h>

uint16_t checksum(uint8_t* packet, size_t head_len);
uint16_t checksum(uint8_t* packet);
bool validateIPChecksum(uint8_t *packet, size_t len);

uint16_t checksum_udp(uint8_t* packet);

#endif
