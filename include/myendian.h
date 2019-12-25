#ifndef ENDING_H_
#define ENDING_H_

#include <stdint.h>

uint16_t rbe16(const uint8_t* a);
uint32_t rbe32(const uint8_t* a);

void wbe16(uint8_t* a, uint16_t x);

#endif
