#ifndef ENDING_H_
#define ENDING_H_

#include <stdint.h>

uint16_t be16(const uint8_t* a);
void wbe16(uint8_t* a, uint16_t x);

uint32_t be2le32(uint32_t x);

#endif
