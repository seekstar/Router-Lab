#include <cstring>

#include "icmp.h"
#include "myip.h"
#include "board.h"
#include "checksum.h"

//Fill ip and udp header of icmp
uint32_t icmp_unreachable(uint8_t* ip, uint32_t if_index, in_addr_t dst_ip, const uint8_t* ori) {
  //Fill ip headers
  ip[0] = 0x45;
  ip[1] = 0;  //from wireshark. why?
  uint16_t total_length = 20 + 8 + ip_head_len(ori);
  *(uint16_t*)(ip + 2) = htobe16(total_length); //total length
  *(uint16_t*)(ip + 4) = htobe16(++ip_id_counter);  //id
  *(uint16_t*)(ip + 6) = 0; //can fragment
  ip[8] = 0xff; //TTL
  ip[9] = 1;  //ICMP;
  //skip checksum
  *(uint32_t*)(ip + 12) = addrs[if_index];  //source
  *(uint32_t*)(ip + 16) = dst_ip;
  fill_ip_checksum(ip);

  //ICMP header
  ip = ip + 20;
  ip[0] = 3; //destination unreachable
  ip[1] = 2; //protocol unreachable
  //skip checksum
  *(uint32_t*)(ip + 4) = 0; //unused

  //ICMP data
  // The original ip header
  uint32_t icmp_data_len = ip_head_len(ori);
  memcpy(ip + 8, ori, icmp_data_len);
  //The ICMP checksum include ICMP header and ICMP data
  *(uint16_t*)(ip + 2) = checksum_be(ip, icmp_data_len + 8, 2);

  return total_length;
}
