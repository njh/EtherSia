/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct ether_header {
    uint8_t dest[6];
    uint8_t src[6];
    uint16_t type;
} __attribute__((__packed__));

#define ETHERTYPE_IPV6		0x86dd

#define IP6_PROTO_TCP   6
#define IP6_PROTO_UDP   17
#define IP6_PROTO_ICMP6 58

#define ICMP6_TYPE_ECHO           128
#define ICMP6_TYPE_ECHO_REPLY     129
#define ICMP6_TYPE_NS             135
#define ICMP6_TYPE_NA             136

#define ICMP6_FLAG_S (1 << 6)

#define ICMP6_OPTION_SOURCE_LINK_ADDRESS 1
#define ICMP6_OPTION_TARGET_LINK_ADDRESS 2


/*
 * Definition for internet protocol version 6.
 * RFC 2460
 */

struct ip6_header {
    uint8_t  ver_tc;     // 4-bits of version, 4-bits of traffic class
    uint8_t  tc_fl;      // 4-bits of traffic class, 4-bits of flow label
    uint16_t flow_label; // Remaining 16-bits of 20-bit flow label
    uint16_t length;     // Payload length (not including this header)
    uint8_t  proto;      // Identifies the type of header immediately following
    uint8_t  hop_limit;  // Decremented by 1 by each node that forwards the packet
    uint8_t  src[16];
    uint8_t  dest[16];
} __attribute__((__packed__));



struct icmp6_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint8_t flags;

    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;

    uint8_t target[16];
    uint8_t option_type;
    uint8_t option_len;
    uint8_t option_mac[6];


} __attribute__((__packed__));
