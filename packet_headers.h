#ifndef ETHERSIA_PACKET_HEADERS_H
#define ETHERSIA_PACKET_HEADERS_H

/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct ether_header {
    MACAddress dest;
    MACAddress src;
    uint16_t type;
} __attribute__((__packed__));
#define ETHER_HEADER_LEN          (14)
#define ETHER_HEADER_OFFSET       (0)
#define ETHER_HEADER              ((struct ether_header*)(buffer + ETHER_HEADER_OFFSET))

static_assert(sizeof(struct ether_header) == ETHER_HEADER_LEN, "Size is not correct");

#define ETHER_TYPE_IPV6           0x86dd

#define IP6_PROTO_TCP             6
#define IP6_PROTO_UDP             17
#define IP6_PROTO_ICMP6           58

#define ICMP6_TYPE_ECHO           128
#define ICMP6_TYPE_ECHO_REPLY     129
#define ICMP6_TYPE_RS             133
#define ICMP6_TYPE_RA             134
#define ICMP6_TYPE_NS             135
#define ICMP6_TYPE_NA             136

#define ICMP6_FLAG_S              (1 << 6)

#define ICMP6_OPTION_SOURCE_LINK_ADDRESS 1
#define ICMP6_OPTION_TARGET_LINK_ADDRESS 2
#define ICMP6_OPTION_PREFIX_INFORMATION  3
#define ICMP6_OPTION_REDIRECTED_HEADER   4
#define ICMP6_OPTION_MTU                 5


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
    IPv6Address src;
    IPv6Address dest;
} __attribute__((__packed__));
#define IP6_HEADER_LEN            (40)
#define IP6_HEADER_OFFSET         (ETHER_HEADER_OFFSET + ETHER_HEADER_LEN)
#define IP6_HEADER                ((struct ip6_header*)(buffer + IP6_HEADER_OFFSET))

static_assert(sizeof(struct ip6_header) == IP6_HEADER_LEN, "Size is not correct");


struct icmp6_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
} __attribute__((__packed__));
#define ICMP6_HEADER_LEN          (4)
#define ICMP6_HEADER_OFFSET       (IP6_HEADER_OFFSET + IP6_HEADER_LEN)
#define ICMP6_HEADER              ((struct icmp6_header*)(buffer + ICMP6_HEADER_OFFSET))

static_assert(sizeof(struct icmp6_header) == ICMP6_HEADER_LEN, "Size is not correct");


struct icmp6_rs_header {
    uint8_t reserved[4];

    uint8_t option_type;
    uint8_t option_len;
    MACAddress option_mac;
} __attribute__((__packed__));
#define ICMP6_RS_HEADER_LEN       (12)
#define ICMP6_RS_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)
#define ICMP6_RS_HEADER           ((struct icmp6_rs_header*)(buffer + ICMP6_RS_HEADER_OFFSET))

static_assert(sizeof(struct icmp6_rs_header) == ICMP6_RS_HEADER_LEN, "Size is not correct");

struct icmp6_ra_header {
    uint8_t current_hop_limit;
    uint8_t flags;
    uint16_t router_lifetime;

    uint32_t reachable_time;
    uint32_t retrans_timer;
} __attribute__((__packed__));
#define ICMP6_RA_HEADER_LEN       (12)
#define ICMP6_RA_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)
#define ICMP6_RA_HEADER           ((struct icmp6_ra_header*)(buffer + ICMP6_RA_HEADER_OFFSET))

static_assert(sizeof(struct icmp6_ra_header) == ICMP6_RA_HEADER_LEN, "Size is not correct");


struct icmp6_prefix_information {
    uint8_t prefix_length;
    uint8_t flags;

    uint32_t valid_lifetime;
    uint32_t preffered_lifetime;
    uint32_t reserved;

    IPv6Address prefix;
} __attribute__((__packed__));


struct icmp6_ns_header {
    uint8_t reserved[4];
    IPv6Address target;
} __attribute__((__packed__));
#define ICMP6_NS_HEADER_LEN       (20)
#define ICMP6_NS_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)
#define ICMP6_NS_HEADER           ((struct icmp6_ns_header*)(buffer + ICMP6_NS_HEADER_OFFSET))

static_assert(sizeof(struct icmp6_ns_header) == ICMP6_NS_HEADER_LEN, "Size is not correct");


struct icmp6_na_header {
    uint8_t flags;
    uint8_t reserved[3];
    IPv6Address target;

    uint8_t option_type;
    uint8_t option_len;
    MACAddress option_mac;
} __attribute__((__packed__));
#define ICMP6_NA_HEADER_LEN       (28)
#define ICMP6_NA_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)
#define ICMP6_NA_HEADER           ((struct icmp6_na_header*)(buffer + ICMP6_NA_HEADER_OFFSET))

static_assert(sizeof(struct icmp6_na_header) == ICMP6_NA_HEADER_LEN, "Size is not correct");


struct udp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((__packed__));
#define UDP_HEADER_LEN            (8)
#define UDP_HEADER_OFFSET         (IP6_HEADER_OFFSET + IP6_HEADER_LEN)
#define UDP_HEADER                ((struct udp_header*)(buffer + UDP_HEADER_OFFSET))
#define UDP_PAYLOAD_PTR           (char*)(buffer + UDP_HEADER_OFFSET + UDP_HEADER_LEN)

static_assert(sizeof(struct udp_header) == UDP_HEADER_LEN, "Size is not correct");


#endif
