#include "ending.h"

uint16_t be16(const uint8_t* a) {
    return ((uint16_t)a[0] << 8) | a[1];
}
