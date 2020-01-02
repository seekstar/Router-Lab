#ifndef RIP_H_
#define RIP_H_

#include <stdint.h>

#include "lookup.h"
#include "router_hal.h"
#include "checksum.h"

#define RIP_MAX_ENTRY 25

extern const in_addr_t RIP_IP_BE;
extern const uint8_t RIP_MAC_BE[];

typedef struct {
  // all fields are big endian
  // we don't store 'family', as it is always 2(response) and 0(request)
  // we don't store 'tag', as it is always 0
  uint32_t addr;
  uint32_t mask;
  uint32_t nexthop;
  uint32_t metric;
} RipEntry;

typedef struct {
  uint32_t numEntries;
  // all fields below are big endian
  uint8_t command;  //1 for request, 2 for response
  // we don't store 'version', as it is always 2
  // we don't store 'zero', as it is always 0
  RipEntry entries[RIP_MAX_ENTRY];
} RipPacket;

void RipUpdateRT(const RipPacket& rip, size_t n, uint32_t src_ip, uint32_t if_index);
void GetRoutingTable(RipPacket& rip);
uint32_t fill_header_of_rip(uint8_t* ip, uint32_t if_index, uint32_t n);

#endif
