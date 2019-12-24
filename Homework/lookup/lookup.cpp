#include "router.h"
#include <stdint.h>
#include <stdlib.h>

#include <list>

//#include "ending.h"

using namespace std;

/*
  RoutingTable Entry 的定义如下：
  typedef struct {
    uint32_t addr; // 大端序，IPv4 地址
    uint32_t len; // 小端序，前缀长度
    uint32_t if_index; // 小端序，出端口编号
    uint32_t nexthop; // 大端序，下一跳的 IPv4 地址
  } RoutingTableEntry;

  约定 addr 和 nexthop 以 **大端序** 存储。
  这意味着 1.2.3.4 对应 0x04030201 而不是 0x01020304。
  保证 addr 仅最低 len 位可能出现非零。
  当 nexthop 为零时这是一条直连路由。
  你可以在全局变量中把路由表以一定的数据结构格式保存下来。
*/

list<RoutingTableEntry> routing_table;
const uint32_t masks[33] = {
  0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff,
  0x80ff, 0xc0ff, 0xe0ff, 0xf0ff, 0xf8ff, 0xfcff, 0xfeff, 0xffff,
  0x80ffff, 0xc0ffff, 0xe0ffff, 0xf0ffff, 0xf8ffff, 0xfcffff, 0xfeffff, 0xffffff,
  0x80ffffff, 0xc0ffffff, 0xe0ffffff, 0xf0ffffff, 0xf8ffffff, 0xfcffffff, 0xfeffffff, 0xffffffff,
};

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
  return entry.addr == (addr & masks[entry.len]);
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
      found = true;
      *if_index = it->if_index;
      *nexthop = it->nexthop;
      mx_len = it->len;
    }
  }
  return found;
}
