#include <endian.h>

#include "myendian.h"

//read big endian(16 bits) to host
uint16_t rbe16(const uint8_t* a) {
    return be16toh(*(uint16_t*)a);
}
uint32_t rbe32(const uint8_t* a) {
    return be32toh(*(uint32_t*)a);
}

//write host 16 bits to big endian 
void wbe16(uint8_t* a, uint16_t x) {
    *(uint16_t*)a = htobe16(x);
}
