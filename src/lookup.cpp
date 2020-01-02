#include "lookup.h"
#include <stdint.h>
#include <stdlib.h>

#include "mask.h"
#include "myip.h"

#define DEBUG 0

using namespace std;

list<RoutingTableEntry> routing_table;

/**
 * @brief 插入/删除一条路由表表项
 * @param insert 如果要插入则为 true ，要删除则为 false
 * @param entry 要插入/删除的表项
 * 
 * 插入时如果已经存在一条 addr 和 len 都相同的表项，则替换掉原有的。
 * 删除时按照 addr 和 len 匹配。
 */
void update(bool insert, RoutingTableEntry entry) {
  // TODO:
  auto it = routing_table.begin();
  for (; it != routing_table.end() && (it->addr != entry.addr || it->len != entry.len); ++it);

  if (insert) {
    if (it != routing_table.end()) {
      it->if_index = entry.if_index;
      it->nexthop = entry.nexthop;
    } else {
      routing_table.push_front(entry);
    }
  } else {
    if (it != routing_table.end()) {
      routing_table.erase(it);
    }
  }
}

bool match(uint32_t addr, const RoutingTableEntry& entry) {
  return entry.addr == (addr & masks_be[entry.len]);
}

/**
 * @brief 进行一次路由表的查询，按照最长前缀匹配原则
 * @param addr 需要查询的目标地址，大端序
 * @param nexthop 如果查询到目标，把表项的 nexthop 写入
 * @param if_index 如果查询到目标，把表项的 if_index 写入
 * @return 查到则返回 true ，没查到则返回 false
 */
bool query(uint32_t addr, uint32_t *nexthop, uint32_t *if_index) {
  // TODO:
  bool found = false;
  uint32_t mx_len = 0;
  for (auto it = routing_table.begin(); it != routing_table.end(); ++it) {
    if (it->len > mx_len && match(addr, *it)) {
#if DEBUG
      printf("query: matched address for ");
      print_ip(stdout, addr);
      printf(" is ");
      print_ip(stdout, it->addr);
      putchar('\n');
#endif
      found = true;
      *if_index = it->if_index;
      *nexthop = it->nexthop;
      mx_len = it->len;
    }
  }
  return found;
}

void print_routing_table(FILE* out) {
  for (const RoutingTableEntry& entry : routing_table) {
    fprintf(out, "{addr = ");
    print_ip(out, entry.addr);
    fprintf(out, ", len = %d, if_index = %d, nexthop = ", entry.len, entry.if_index);
    print_ip(out, entry.nexthop);
    fprintf(out, ", metric = %d", entry.metric);
    fprintf(out, "}\n");
  }
  fputc('\n', out);
}
