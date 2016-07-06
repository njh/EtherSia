#ifndef IPV6PACKET_H
#define IPV6PACKET_H

#include <stdint.h>
#include "MACAddress.h"
#include "IPv6Address.h"


#define ETHER_TYPE_IPV6           0x86dd

#define IP6_DEFAULT_HOP_LIMIT     (64)

#define IP6_PROTO_TCP             6
#define IP6_PROTO_UDP             17
#define IP6_PROTO_ICMP6           58


class IPv6Packet {

public:
    IPv6Packet();

    uint8_t* payload();
    void init();
    uint16_t calculateChecksum();

    // Ethernet Header
    MACAddress etherDest;
    MACAddress etherSrc;
    uint16_t etherType;

    // IPv6 Header
    uint8_t  ver_tc;     // 4-bits of version, 4-bits of traffic class
    uint8_t  tc_fl;      // 4-bits of traffic class, 4-bits of flow label
    uint16_t flowLabel;  // Remaining 16-bits of 20-bit flow label
    uint16_t length;     // Payload length (not including this header)
    uint8_t  proto;      // Identifies the type of header immediately following
    uint8_t  hopLimit;   // Decremented by 1 by each node that forwards the packet
    IPv6Address src;
    IPv6Address dest;

}  __attribute__((__packed__));

#define ETHER_HEADER_LEN          (14)
#define IP6_HEADER_LEN            (40)

static_assert(sizeof(IPv6Packet) == ETHER_HEADER_LEN + IP6_HEADER_LEN, "Size is not correct");


#endif
