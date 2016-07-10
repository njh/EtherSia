/** 
 * Header file for the IPv6Packet class
 * @file IPv6Packet.h
 */

#ifndef IPV6PACKET_H
#define IPV6PACKET_H

#include <stdint.h>
#include "MACAddress.h"
#include "IPv6Address.h"

/** Enumeration of Ethernet frame types */
enum ether_types {
    ETHER_TYPE_IPV6 = 0x86dd    ///< Ethernet type for IPv6
};

/** Default value for the IPv6 hop limit field */
#define IP6_DEFAULT_HOP_LIMIT     (64)

/** Enumeration of IP protocol numbers */
enum ip_protocol {
    IP6_PROTO_TCP = 6,      ///< IP protocol number for TCP
    IP6_PROTO_UDP = 17,     ///< IP protocol number for UDP 
    IP6_PROTO_ICMP6 = 58    ///< IP protocol number for ICMP6
};


/**
 * Class for getting and setting the fields of an IPv6 Packet and its Ethernet header
 */
class IPv6Packet {

public:
    /**
     * Constructor for new empty IPv6 packet
     */
    IPv6Packet();

    /**
     * Initialise default values for IPv6 packet and Ethernet frame header
     *
     * Sets:
     * - EtherType to IPv6
     * - IP version to 6
     * - Traffic Class to 0
     * - Flow Label to 0
     * - Hop Limit to default value
     */
    void init();

    /**
     * Get a pointer to the start of the IPv6 packet payload
     */
    uint8_t* payload();

    /**
     * Check if the Ethernet and IPv6 headers are valid
     */
    boolean isValid();

    /**
     * Calculate the 16-bit checksum for the IPv6 packet
     */
    uint16_t calculateChecksum();

    // Ethernet Header
    MACAddress etherDest;  ///< The Ethernet frame destination MAC address
    MACAddress etherSrc;   ///< The Ethernet frame source MAC address
    uint16_t etherType;    ///< 16-bit Ethernet frame type field (aka EtherType)

    // IPv6 Header
    uint8_t  ver_tc;       ///< 4-bits of version, 4-bits of traffic class
    uint8_t  tc_fl;        ///< 4-bits of traffic class, 4-bits of flow label
    uint16_t flowLabel;    ///< Remaining 16-bits of 20-bit flow label
    uint16_t length;       ///< Payload length (not including this header)
    uint8_t  proto;        ///< Identifies the type of header immediately following
    uint8_t  hopLimit;     ///< Decremented by 1 by each node that forwards the packet
    IPv6Address src;       ///< The IPv6 packet source address
    IPv6Address dest;      ///< The IPv6 packet destination address

}  __attribute__((__packed__));

/** The length of an Ethernet frame header */
#define ETHER_HEADER_LEN          (14)

/** The length of an IPv6 packet header */
#define IP6_HEADER_LEN            (40)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(IPv6Packet) == ETHER_HEADER_LEN + IP6_HEADER_LEN, "Size is not correct");


#endif
