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
