/**
 * Header file for the ICMPv6Packet class
 * @file ICMPv6Packet.h
 */

#ifndef ICMPV6_PACKET_H
#define ICMPV6_PACKET_H

#define ICMP6_TYPE_UNREACHABLE    1
#define ICMP6_TYPE_PARAM_PROB     4
#define ICMP6_TYPE_ECHO           128
#define ICMP6_TYPE_ECHO_REPLY     129
#define ICMP6_TYPE_RS             133
#define ICMP6_TYPE_RA             134
#define ICMP6_TYPE_NS             135
#define ICMP6_TYPE_NA             136
#define ICMP6_TYPE_NA             136

#define ICMP6_CODE_PORT_UNREACHABLE  4
#define ICMP6_CODE_UNRECOGNIZED_NH   1

#define ICMP6_NA_FLAG_S           (1 << 6)

#define ICMP6_OPTION_SOURCE_LINK_ADDRESS 1
#define ICMP6_OPTION_TARGET_LINK_ADDRESS 2
#define ICMP6_OPTION_PREFIX_INFORMATION  3
#define ICMP6_OPTION_REDIRECTED_HEADER   4
#define ICMP6_OPTION_MTU                 5
#define ICMP6_OPTION_RECURSIVE_DNS       25


/* The length of the header of an ICMPv6 packet */
#define ICMP6_HEADER_LEN          (4)
#define ICMP6_HEADER_OFFSET       (ETHER_HEADER_LEN + IP6_HEADER_LEN)


/**
 * Structure for accessing the fields of a ICMP6 Error packet
 * @private
 */
struct icmp6_error_header {
    union {
        uint32_t unused;
        uint32_t pointer;
        uint32_t mtu;
    } __attribute__((__packed__));
    // IPv6 Packet payload follows
} __attribute__((__packed__));
#define ICMP6_ERROR_HEADER_LEN       (4)
#define ICMP6_ERROR_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct icmp6_error_header) == ICMP6_ERROR_HEADER_LEN, "Size is not correct");


/**
 * Structure for accessing the fields of a ICMP6 Echo Request/Reply packet
 * @private
 */
struct icmp6_echo_header {
    uint16_t identifier;
    uint16_t sequenceNumber;
} __attribute__((__packed__));
#define ICMP6_ECHO_HEADER_LEN     (4)
#define ICMP6_ECHO_HEADER_OFFSET  (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct icmp6_echo_header) == ICMP6_ECHO_HEADER_LEN, "Size is not correct");



/**
 * Structure for options that store a Link address
 * @private
 */
struct icmp6_option_mac {
    uint8_t type;
    uint8_t len;
    MACAddress mac;
} __attribute__((__packed__));
#define ICMP6_OPTION_MAC_LEN       (8)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct icmp6_option_mac) == ICMP6_OPTION_MAC_LEN, "Size is not correct");


/**
 * Structure for accessing the fields of a ICMP6 Router Solicitation packet
 * @private
 */
struct icmp6_rs_header {
    uint8_t reserved[4];

    struct icmp6_option_mac option1;
} __attribute__((__packed__));
#define ICMP6_RS_HEADER_LEN       (12)
#define ICMP6_RS_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct icmp6_rs_header) == ICMP6_RS_HEADER_LEN, "Size is not correct");

/**
 * Structure for accessing the fields of a ICMP6 Router Advertisement packet
 * @private
 */
struct icmp6_ra_header {
    uint8_t current_hop_limit;
    uint8_t flags;
    uint16_t router_lifetime;

    uint32_t reachable_time;
    uint32_t retrans_timer;
} __attribute__((__packed__));
#define ICMP6_RA_HEADER_LEN       (12)
#define ICMP6_RA_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct icmp6_ra_header) == ICMP6_RA_HEADER_LEN, "Size is not correct");


/**
 * Structure for accessing the fields of a ICMP6 Prefix Information data
 * @private
 */
struct icmp6_prefix_information {
    uint8_t prefix_length;
    uint8_t flags;

    uint32_t valid_lifetime;
    uint32_t preffered_lifetime;
    uint32_t reserved;

    IPv6Address prefix;
} __attribute__((__packed__));


/**
 * Structure for accessing the fields of a ICMP6 Neighbour Solicitation packet
 * @private
 */
struct icmp6_ns_header {
    uint8_t reserved[4];
    IPv6Address target;

    struct icmp6_option_mac option1;
} __attribute__((__packed__));
#define ICMP6_NS_HEADER_LEN       (28)
#define ICMP6_NS_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct icmp6_ns_header) == ICMP6_NS_HEADER_LEN, "Size is not correct");


/**
 * Structure for accessing the fields of a ICMP6 Neighbour Advertisement packet
 * @private
 */
struct icmp6_na_header {
    uint8_t flags;
    uint8_t reserved[3];
    IPv6Address target;

    struct icmp6_option_mac option1;
} __attribute__((__packed__));
#define ICMP6_NA_HEADER_LEN       (28)
#define ICMP6_NA_HEADER_OFFSET    (ICMP6_HEADER_OFFSET + ICMP6_HEADER_LEN)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct icmp6_na_header) == ICMP6_NA_HEADER_LEN, "Size is not correct");



/**
 * Class for accessing the fields of a ICMP6 packet
 * @private
 */
class ICMPv6Packet : public IPv6Packet {

public:
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    union {
        struct icmp6_error_header err;
        struct icmp6_echo_header echo;
        struct icmp6_ra_header ra;
        struct icmp6_rs_header rs;
        struct icmp6_na_header na;
        struct icmp6_ns_header ns;
    } __attribute__((__packed__));

} __attribute__((__packed__));


#endif
