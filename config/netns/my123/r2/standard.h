#include "router_hal.h"

// configure this to match the output of `ip a`
const char *interfaces[N_IFACE_ON_BOARD] = {
    "r2r1",
    "r2r3",
    "eth3",
    "eth4",
};
