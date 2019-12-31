#ifndef LOOKUP_H_
#define LOOKUP_H_

#include <list>
#include "router.h"

using namespace std;

extern list<RoutingTableEntry> routing_table;

void update(bool insert, RoutingTableEntry entry);
bool query(uint32_t addr, uint32_t *nexthop, uint32_t *if_index);

#endif
