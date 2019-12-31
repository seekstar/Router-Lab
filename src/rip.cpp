#include <algorithm>

#include "rip.h"
#include "board.h"
#include "mask.h"
#include "myip.h"

using namespace std;

#define INF_METRIC 16u

const in_addr_t RIP_IP_BE = 0xe0000009;

//#define RIP_MAC_BE 0x0900005e0001
const uint8_t RIP_MAC_BE[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x09};

bool exact_match(const RipEntry& re, const RoutingTableEntry& te) {
    return re.addr == te.addr && re.mask == masks_be[te.len];
}
// n rip entries
void RipUpdateRT(const RipPacket& rip, size_t n, uint32_t if_index) {
    for (size_t i = 0; i < n; ++i) {
        auto& re = rip.entries[i];
        auto it = routing_table.begin();
        for (it = routing_table.begin(); it != routing_table.end(); ++it) {
            if (exact_match(re, *it)) {
                break;
            }
        }
        uint32_t new_metric = min(re.metric + 1, INF_METRIC);
        if (it != routing_table.end()) {
            auto& te = *it;
            /*optional: if (addrs[if_index] == te.nexthop) {
                reinit the timeout
            }*/
            if ((addrs[if_index] == te.nexthop && new_metric != te.metric) || (new_metric < te.metric)) {
                if (INF_METRIC == new_metric) {
                    //optional: deletion process
                    routing_table.erase(it);
                } else {
                    te.nexthop = addrs[if_index];
                    te.metric = new_metric;
                }
            }
        } else {
            if (re.metric != INF_METRIC) {
                routing_table.push_back({re.addr, mask_be_len[re.mask], if_index, addrs[if_index], new_metric});
            }
        }
    }
}

void GetRoutingTable(RipPacket& rip) {
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
