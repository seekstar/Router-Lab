#include <algorithm>
#include <endian.h>

#include "rip.h"
#include "board.h"
#include "mask.h"
#include "myip.h"
#include "myendian.h"

using namespace std;

#define DEBUG 1

#define INF_METRIC 16u

const in_addr_t RIP_IP_LE = 0xe0000009;
const in_addr_t RIP_IP_BE = 0x090000e0;

//#define RIP_MAC_BE 0x0900005e0001
const uint8_t RIP_MAC_BE[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x09};

bool exact_match(const RipEntry& re, const RoutingTableEntry& te) {
    return re.addr == te.addr && re.mask == masks_be[te.len];
}
// n rip entries
//src ip is the source ip of the rip packet
void RipUpdateRT(const RipPacket& rip, size_t n, uint32_t src_ip, uint32_t if_index) {
    for (size_t i = 0; i < n; ++i) {
        auto& re = rip.entries[i];
        auto it = routing_table.begin();
        for (it = routing_table.begin(); it != routing_table.end(); ++it) {
            if (exact_match(re, *it)) {
                break;
            }
        }
        uint32_t new_metric = min(be32toh(re.metric) + 1, INF_METRIC);
        if (it != routing_table.end()) {
            auto& te = *it;
#if DEBUG
            printf("RipUpdateRT: match (addr = ");
            print_ip(stdout, te.addr);
            printf(", len = %d) for (addr = ", te.len);
            print_ip(stdout, re.addr);
            printf(", mask = ");
            print_ip(stdout, re.mask);
            printf(")\n");
#endif
            /*optional: if (src_ip == te.nexthop) {
                reinit the timeout
            }*/
            if ((src_ip == te.nexthop && new_metric != te.metric) || (new_metric < te.metric)) {
                if (INF_METRIC == new_metric) {
                    //optional: deletion process
                    routing_table.erase(it);
                } else {
                    te.nexthop = src_ip;
                    te.metric = new_metric;
                }
            }
        } else {
#if DEBUG
            printf("A new routing table entry?\n");
#endif
            if (new_metric != INF_METRIC) {
#if DEBUG
                printf("New routing table entry is added\n");
#endif
                routing_table.push_back({re.addr, mask_be_len[re.mask], if_index, src_ip, new_metric});
            }
#if DEBUG
            else {
                printf("The metric is infinity %d\n", new_metric);
            }
#endif
        }
    }
}

//reverse poisoning
void GetRoutingTable(RipPacket& rip, uint32_t if_index) {
  rip.numEntries = routing_table.size();
  rip.command = 2;  //response
  uint32_t i = 0;
  for (const RoutingTableEntry& entry : routing_table) {
    rip.entries[i++] = {
      .addr = entry.addr,
      .mask = masks_be[entry.len],
      .nexthop = entry.nexthop,
      .metric = htobe32(entry.if_index == if_index ? INF_METRIC : entry.metric)        //big endian
    };
  }
}
//Fill ip and udp header of rip
//n rip entries
uint32_t fill_header_of_rip(uint8_t* ip, uint32_t if_index, uint32_t n) {
  // Fill IP headers
  ip[0] = 0x45;
  ip[1] = 0xc0;
  wbe16(ip + 2, 20 + 8 + 4 + n * 20);     //Total Length
  wbe16(ip + 4, ++ip_id_counter);   //identification
  wbe16(ip + 6, 0x4000);   //Flags: Don't fragment
  ip[8] = 1;    //TTL
  ip[9] = 0x11; //UDP
  //skip checksum
  *(uint32_t*)(ip + 12) = addrs[if_index]; //source
  *(uint32_t*)(ip + 16) = RIP_IP_BE; //Destination
  fill_ip_checksum(ip);

  // Fill UDP headers
  ip += 20;
  // port = 520
  *(uint16_t*)ip = htobe16(520);  //source port
  *(uint16_t*)(ip + 2) = htobe16(520);  //destination port
  *(uint16_t*)(ip + 4) = htobe16(8 + 4 + n * 20);  //Length
  fill_udp_checksum(ip);

  return 20 + 8;
}
