#include "myip.h"

uint32_t ip_head_len(const uint8_t* ip) {
    return (*ip & 0xf) * 4;
}
