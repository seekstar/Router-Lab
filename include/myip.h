#ifndef MYIP_H_
#define MYIP_H_

#include <inttypes.h>
#include <stdio.h>

#include "router_hal.h"

extern uint16_t ip_id_counter;

uint32_t ip_head_len(const uint8_t* ip);
void print_ip(FILE* out, in_addr_t ip);

#endif
