#include "router_hal.h"

// configure this to match the output of `ip a`
const char *interfaces[N_IFACE_ON_BOARD] = {
    "r1pc1",
    "r1r2",
    "eth3",
    "eth4",
};
