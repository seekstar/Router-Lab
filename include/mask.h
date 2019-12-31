#ifndef MASK_H_
#define MASK_H_

#include <cinttypes>
#include <unordered_map>

using namespace std;

extern const uint32_t masks_be[33];

struct mask_be_len_map {
    unordered_map<uint32_t, uint32_t> ma;
    mask_be_len_map();
    uint32_t operator [] (uint32_t mask);
};
extern mask_be_len_map mask_be_len;

#endif
