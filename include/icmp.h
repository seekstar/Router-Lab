#ifndef __ICMP_H__
#define __ICMP_H__

#include <cinttypes>

#include "router_hal.h"

uint32_t icmp_unreachable(uint8_t* ip, uint32_t if_index, in_addr_t dst_ip, const uint8_t* ori);

#endif
