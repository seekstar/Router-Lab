#include "rip.h"
#include <stdio.h>
#include <string.h>

#include "checksum.h"
#include "lookup.h"
#include "board.h"
#include "myip.h"
#include "icmp.h"
#include "mac.h"

using namespace std;

#define DEBUG 1

// when testing, you can change 30s to 5s
//timer for sending routing table
#define TIMER_SRT 5

extern bool forward(uint8_t *packet, size_t len);
extern bool disassemble(const uint8_t *packet, uint32_t len, RipPacket *output);
extern uint32_t assemble(const RipPacket *rip, uint8_t *buffer);

uint8_t packet[2048];
uint8_t output[2048];

int main(int argc, char *argv[]) {
  // 0a.
  int res = HAL_Init(1, addrs);
  if (res < 0) {
    return res;
  }

  // 0b. Add direct routes
  // For example:
  // 192.168.1.1/24 if 0
  // 192.168.5.2/24 if 1
  // 10.0.2.0/24 if 2
  // 10.0.3.0/24 if 3
  for (uint32_t i = 0; i < N_IFACE_ON_BOARD; i++) {
    RoutingTableEntry entry = {
        .addr = addrs[i] & 0x00FFFFFF, // big endian
        .len = 24,        // small endian
        .if_index = i,    // small endian
        .nexthop = 0,      // big endian, means direct
        .metric = 1   //direct
    };
    update(true, entry);
  }

  uint64_t last_time = 0;
  while (1) {
    uint64_t time = HAL_GetTicks();
    if (time > last_time + TIMER_SRT * 1000) {
      // DONE: send complete routing table to every interface
      // ref. RFC2453 Section 3.8
      // multicast MAC for 224.0.0.9 is 01:00:5e:00:00:09
      printf("%ds Timer\n", TIMER_SRT);

      RipPacket rip;
      GetRoutingTable(rip);
      //can optimize
      uint32_t len = assemble(&rip, packet + 20 + 8);
      for (uint32_t i = 0; i < N_IFACE_ON_BOARD; ++i) {
        fill_header_of_rip(packet, i, rip.numEntries);
        HAL_SendIPPacket(i, packet, len + 28, RIP_MAC_BE);
      }

      // DONE: print complete routing table to stdout/stderr
      fprintf(stderr, "Entire routing table:\n");
      print_routing_table(stderr);

      last_time = time;
    }

    int mask = (1 << N_IFACE_ON_BOARD) - 1;
    macaddr_t src_mac;
    macaddr_t dst_mac;
    int if_index;
    res = HAL_ReceiveIPPacket(mask, packet, sizeof(packet), src_mac, dst_mac,
                              1000, &if_index);
    if (res == HAL_ERR_EOF) {
      break;
    } else if (res < 0) {
      return res;
    } else if (res == 0) {
      // Timeout
      continue;
    } else if (res > sizeof(packet)) {
      // packet is truncated, ignore it
      continue;
    }

    // 1. validate
    if (!validateIPChecksum(packet, res)) {
      printf("Invalid IP Checksum\n");
      continue;
    }
    in_addr_t src_addr, dst_addr;
    // DONE: extract src_addr and dst_addr from packet (big endian)
    src_addr = *(in_addr_t*)(packet + 12);
    dst_addr = *(in_addr_t*)(packet + 16);
    
#if DEBUG
    printf("if_index = %d, src_addr = ", if_index);
    print_ip(stdout, src_addr);
    printf(", dst_addr = ");
    print_ip(stdout, dst_addr);
    putchar('\n');
#endif

    // 2. check whether dst is me
    bool dst_is_me = false;
    for (int i = 0; i < N_IFACE_ON_BOARD; i++) {
      if (memcmp(&dst_addr, &addrs[i], sizeof(in_addr_t)) == 0) {
        dst_is_me = true;
        break;
      }
    }
    // DONE: handle rip multicast address(224.0.0.9)
    if (!dst_is_me && dst_addr == RIP_IP_BE) {
      dst_is_me = true;
    }

    if (dst_is_me) {
#if DEBUG
      printf("An ip packet for me\n");
#endif
      if (dst_addr == RIP_IP_BE) {
        // 3a.1
        RipPacket rip;
        // check and validate
        if (disassemble(packet, res, &rip)) {
  #if DEBUG
          printf("Valid rip packet received\n");
  #endif
          if (rip.command == 1) { //request
            // 3a.3 request, ref. RFC2453 Section 3.9.1
            // only need to respond to whole table requests in the lab

            if (rip.numEntries) { //If there are no entries, no response is given.
              RipPacket resp;
              // DONE: fill resp
              GetRoutingTable(resp);
              fill_header_of_rip(output, if_index, routing_table.size());

              // assembleRIP
              uint32_t rip_len = assemble(&resp, &output[20 + 8]);

              // send it back
              HAL_SendIPPacket(if_index, output, rip_len + 20 + 8, src_mac);
            }
          } else {
            // 3a.2 response, ref. RFC2453 Section 3.9.2
            // DONE: update routing table
            // update metric, if_index, nexthop
            // HINT: handle nexthop = 0 case  ?????
            // HINT: what is missing from RoutingTableEntry?
            // you might want to use `query` and `update` but beware of the difference between exact match and longest prefix match
            RipUpdateRT(rip, (res - 20 - 8 - 4) / 20, src_addr, if_index);
            
            printf("routing table updated:\n");
            print_routing_table(stdout);

            // optional: triggered updates? ref. RFC2453 3.10.1
          }
        }
      } else {
        printf("The ip packet for me is not a rip packet, ignore it\n");
      }
    } else {
      // 3b.1 dst is not me
      // forward
      // beware of endianness
      uint32_t nexthop, dest_if;
      if (query(dst_addr, &nexthop, &dest_if)) {
        // found
        macaddr_t dest_mac;
        // direct routing
        if (nexthop == 0) {
          nexthop = dst_addr;
        }
        if (HAL_ArpGetMacAddress(dest_if, nexthop, dest_mac) == 0) {
          // found
#if DEBUG
          printf("found arp, dest_if = %d, dest_mac = ", dest_if);
          print_mac(stdout, dest_mac);
          putchar('\n');
#endif
          memcpy(output, packet, res);
          // update ttl and checksum
          if (forward(output, res)) {
#if DEBUG
            printf("sent\n");
#endif
            HAL_SendIPPacket(dest_if, output, res, dest_mac);
          }
        } else {
          // not found
          // you can drop it
          printf("ARP not found for nexthop ");
          print_ip(stdout, nexthop);
          printf(", if_index = %d\n", dest_if);
        }
      } else {
        // not found
        // DONE(optional): send ICMP Host Unreachable
        uint32_t len = icmp_unreachable(output, if_index, src_addr, packet);
        HAL_SendIPPacket(if_index, output, len, src_mac);
        printf("IP not found for src ");
        print_ip(stdout, src_addr);
        printf(" dst ");
        print_ip(stdout, dst_addr);
        putchar('\n');
      }
    }
  }
  return 0;
}
