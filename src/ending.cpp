#include "ending.h"

//big ending(16 bits)
uint16_t be16(const uint8_t* a) {
    return ((uint16_t)a[0] << 8) | a[1];
}

//write be16
void wbe16(uint8_t* a, uint16_t x) {
    a[0] = x >> 8;
    a[1] = x & 0xff;
}

uint16_t swap16(uint16_t x) {
    return (x & 0xff) << 8 | (x >> 8);
}
uint32_t swap32(uint32_t x) {
    return swap16(x >> 16) | (swap16(x & 0xffff) << 16);
}
