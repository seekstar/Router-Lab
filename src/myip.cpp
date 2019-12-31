#include <stdio.h>

#include "myip.h"

uint16_t ip_id_counter = 0;

uint32_t ip_head_len(const uint8_t* ip) {
    return (*ip & 0xf) * 4;
}

void print_ip(FILE* out, in_addr_t ip) {
    fprintf(out, "%d.%d.%d.%d", ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff);
}
