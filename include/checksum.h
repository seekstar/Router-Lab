#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include <stdint.h>
#include <stdlib.h>

uint16_t checksum(uint8_t* packet, size_t len);
bool validateIPChecksum(uint8_t *packet, size_t len);

#endif
