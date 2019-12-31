#include <algorithm>

#include "rip.h"
#include "board.h"
#include "mask.h"

using namespace std;

#define INF_METRIC 16u

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
                routing_table.emplace_back(re.addr, mask_be_len[re.mask], if_index, addrs[if_index], new_metric);
            }
        }
    }
}
