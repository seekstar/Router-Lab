#include "rip.h"
#include <stdio.h>
#include <string.h>

#include "checksum.h"
#include "lookup.h"
#include "board.h"
#include "myip.h"

using namespace std;

// when testing, you can change 30s to 5s
//timer for sending routing table
#define TIMER_SRT 30

//#define RIP_MAC_BE 0x0900005e0001
const uint8_t RIP_MAC_BE[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x09};
const in_addr_t RIP_IP_BE = 0xe0000009;

extern bool validateIPChecksum(uint8_t *packet, size_t len);
extern void update(bool insert, RoutingTableEntry entry);
extern bool query(uint32_t addr, uint32_t *nexthop, uint32_t *if_index);
extern bool forward(uint8_t *packet, size_t len);
extern bool disassemble(const uint8_t *packet, uint32_t len, RipPacket *output);
extern uint32_t assemble(const RipPacket *rip, uint8_t *buffer);

extern const uint32_t masks_be[33];

uint8_t packet[2048];
uint8_t output[2048];

uint16_t ip_id_counter = 0;



uint32_t GetRoutingTable(RipPacket& rip) {
  rip.numEntries = routing_table.size();
  rip.command = 2;  //response
  uint32_t i = 0;
  for (const RoutingTableEntry& entry : routing_table) {
    rip.entries[i++] = {
      .addr = entry.addr,
      .mask = masks_be[entry.len],
      .nexthop = entry.nexthop,
      .metric = entry.metric
    };
  }
}
//Fill ip and udp header of rip
//n rip entries
uint32_t fill_header_of_rip(uint8_t* ip, uint32_t if_index, uint32_t n) {
  // Fill IP headers
  *(ip++) = 0x45;
  *(ip++) = 0xc0;
  *(uint16_t*)ip = htobe16(20 + 8 + 4 + n * 20);  //Total Length
  ip += 2;
  *(uint16_t*)ip = htobe16(++ip_id_counter); //identification
  ip += 2;
  *(uint16_t*)ip = 0x4000;  //Flags: Don't fragment
  ip += 2;
  *(ip++) = 1;  //TTL
  *(ip++) = 0x11; //UDP
  ip += 2;  //skip checksum
  *(uint32_t*)ip = addrs[if_index]; //source
  ip += 4;
  *(uint32_t*)ip = RIP_IP_BE; //Destination
  *(uint16_t*)(ip - 6) = checksum(ip - 16);
  ip += 4;

  // Fill UDP headers
  // port = 520
  *(uint16_t*)ip = htobe16(520);  //source port
  *(uint16_t*)(ip + 2) = htobe16(520);  //destination port
  *(uint16_t*)(ip + 4) = htobe16(8 + 4 + n * 20);  //Length
  *(uint16_t*)(ip + 6) = checksum_udp(ip);

  return 20 + 8;
}

//Fill ip and udp header of icmp
uint32_t icmp_unreachable(uint8_t* ip, uint32_t if_index, in_addr_t dst_ip, const uint8_t* ori) {
  //Fill ip headers
  ip[0] = 0x45;
  ip[1] = 0;  //from wireshark. why?
  uint16_t total_length = 20 + 8 + ip_head_len(ori);
  *(uint16_t*)(ip + 2) = htobe16(total_length); //total length
  *(uint16_t*)(ip + 4) = htobe16(++ip_id_counter);  //id
  *(uint16_t*)(ip + 6) = 0; //can fragment
  ip[8] = 0xff; //TTL
  ip[9] = 1;  //ICMP;
  //skip checksum
  *(uint32_t*)(ip + 12) = addrs[if_index];  //source
  *(uint32_t*)(ip + 16) = dst_ip;
  *(uint16_t*)(ip + 10) = checksum(ip);

  //ICMP
  ip = ip + 20;
  ip[0] = 3; //destination unreachable
  ip[1] = 2; //protocol unreachable
  //skip checksum
  *(uint32_t*)(ip + 4) = 0; //unused
  *(uint16_t*)(ip + 2) = checksum(ip, 8, 2);

  // error ip header
  ip += 8;
  memcpy(ip, ori, ip_head_len(ori));

  return total_length;
}

int main(int argc, char *argv[]) {
  // 0a.
  int res = HAL_Init(1, addrs);
  if (res < 0) {
    return res;
  }

  // 0b. Add direct routes
  // For example:
  // 10.0.0.0/24 if 0
  // 10.0.1.0/24 if 1
  // 10.0.2.0/24 if 2
  // 10.0.3.0/24 if 3
  for (uint32_t i = 0; i < N_IFACE_ON_BOARD; i++) {
    RoutingTableEntry entry = {
        .addr = addrs[i] & 0x00FFFFFF, // big endian
        .len = 24,        // small endian
        .if_index = i,    // small endian
        .nexthop = 0      // big endian, means direct
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
      uint32_t len = assemble(&rip, packet + 20 + 8 + 4);
      for (uint32_t i = 0; i < N_IFACE_ON_BOARD; ++i) {
        fill_header_of_rip(packet, i, rip.numEntries);
        HAL_SendIPPacket(i, packet, len, RIP_MAC_BE);
      }

      // DONE: print complete routing table to stdout/stderr
      fprintf(stderr, "Entire routing table:\n");
      for (const RoutingTableEntry& entry : routing_table) {
        fprintf(stderr, "{addr = %p, len = %d, if_index = %d, nexthop = %p}\n", entry.addr, entry.len, entry.if_index, entry.nexthop);
      }
      fputc('\n', stderr);

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
      // 3a.1
      RipPacket rip;
      // check and validate
      if (disassemble(packet, res, &rip)) {
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
          // TODO: update routing table
          // new metric = ?
          // update metric, if_index, nexthop
          // HINT: handle nexthop = 0 case
          // HINT: what is missing from RoutingTableEntry?
          // you might want to use `query` and `update` but beware of the difference between exact match and longest prefix match
          RipUpdateRT(rip, (res - 20 - 8 - 4) / 20, if_index);

          // optional: triggered updates? ref. RFC2453 3.10.1
        }
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
          memcpy(output, packet, res);
          // update ttl and checksum
          if (forward(output, res)) {
            HAL_SendIPPacket(dest_if, output, res, dest_mac);
          }
        } else {
          // not found
          // you can drop it
          printf("ARP not found for nexthop %x\n", nexthop);
        }
      } else {
        // not found
        // DONE(optional): send ICMP Host Unreachable
        uint32_t len = icmp_unreachable(output, if_index, dst_addr, packet);
        HAL_SendIPPacket(if_index, output, len, src_mac);
        printf("IP not found for src %x dst %x\n", src_addr, dst_addr);
      }
    }
  }
  return 0;
}
