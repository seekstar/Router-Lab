#ifndef ROUTER_H_
#define ROUTER_H_

#include <stdint.h>

// 路由表的一项
struct RoutingTableEntry {
    uint32_t addr; // 大端序，IPv4 地址，保证 addr 仅最低 len 位可能出现非零(?)。
    uint32_t len; // host endian，前缀长度
    uint32_t if_index; // host endian，出端口编号
    uint32_t nexthop; // 大端序，下一跳的 IPv4 地址，0 表示直连，注意和 RIP Entry 的 nexthop 区别： RIP 中的 nexthop = 0 表示的是源 IP 地址
    // 为了实现 RIP 协议，需要在这里添加额外的字段
    uint32_t metric;    //host endian
};

#endif
