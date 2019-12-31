#include "mac.h"

void print_mac(FILE* out, const macaddr_t mac) {
	fprintf(out, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
