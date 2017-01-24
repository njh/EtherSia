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
     * May be used to get or change the contents of the packet
     * @return a pointer to the memory containing the packet payload
     */
    uint8_t* payload();

    /**
     * Check if the Ethernet and IPv6 headers are valid
     * Also verifies the checksum of the packet
     *
     * @return true if the packets fields are valid
     */
    boolean isValid();

    /**
     * Marks the packet as being invalid, so that isValid()
     * returns false.
     */
    void invalidate();

    /**
     * Get the Ethernet source MAC address of the packet
     * @return the source address of the packet
     */
    MACAddress& etherSource();

    /**
     * Set the Ethernet source of the packet to a new address
     * @param address the new source address
     */
    void setEtherSource(MACAddress& address);

    /**
     * Get the Ethernet destination MAC address of the packet
     * @return the destination address of the packet
     */
    MACAddress& etherDestination();

    /**
     * Set the Ethernet destination of the packet to a new address
     * @param address the new destination address
     */
    void setEtherDestination(MACAddress& address);

    /**
     * Get the EtherType (Ethernet frame type)
     *
     * @note This should always be 86dd for IPv6
     * @return the 16-bit EtherType of the frame
     */
    uint16_t etherType() const;

    /**
     * Get the IP version number from the IPv6 header.
     * This should always return 6 for a valid packet.
     * @return the version number of the IPv6 header
     */
    uint8_t version() const;

    /**
     * Get total length of this packet, including the Ethernet header,
     * IPv6 header and payload.
     * @return the total length of the IPv6 packet in bytes
     */
    uint16_t length() const;

    /**
     * Get length of this payload of the packet in bytes.
     * This should include everything after the IPv6 header.
     * @return the total length of the packet payload in bytes
     */
    uint16_t payloadLength() const;

    /**
     * Set the length of this payload of the packet in bytes.
     * This should include everything after the IPv6 header.
     * @param length the length of the payload in bytes
     */
    void setPayloadLength(uint16_t length);

    /**
     * Get the protocol number for the packet
     * For example IP6_PROTO_UDP = 17.
     *
     * @sa http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
     * @return the protocol number
     */
    uint8_t protocol();

    /**
     * Set the protocol number for the packet
     * For example IP6_PROTO_UDP = 17.
     *
     * @sa http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
     * @param protocol the new protocol number
     */
    void setProtocol(uint8_t protocol);


    /**
     * Get the hop limit for the packet
     * This is the number of router hops a packet is allowed before being discarded.
     *
     * @return the hop limit for the packet
     */
    uint8_t hopLimit();

    /**
     * Set the hop limit for the packet
     * This is the number of router hops a packet is allowed before being discarded.
     *
     * @param hopLimit the new hop limit for the packet
     */
    void setHopLimit(uint8_t hopLimit);

    /**
     * Get the IPv6 source address of the packet
     * @return the source address of the packet
     */
    IPv6Address& source();

    /**
     * Set the IPv6 source of the packet to a new address
     * @param address the new source address
     */
    void setSource(IPv6Address& address);

    /**
     * Get the IPv6 destination address of the packet
     * @return the destination address of the packet
     */
    IPv6Address& destination();

    /**
     * Set the IPv6 destination of the packet to a new address
     * @param address the new destination address
     */
    void setDestination(IPv6Address& address);

    /**
     * Calculate the 16-bit checksum for the IPv6 packet
     * @return the checksum of the packet
     */
    uint16_t calculateChecksum();

protected:

    // Ethernet Header
    MACAddress _etherDestination; ///< The Ethernet frame destination MAC address
    MACAddress _etherSource;      ///< The Ethernet frame source MAC address
    uint16_t _etherType;          ///< 16-bit Ethernet frame type field (aka EtherType)

    // IPv6 Header
    uint8_t  _ver_tc;         ///< 4-bits of version, 4-bits of traffic class
    uint8_t  _tc_fl;          ///< 4-bits of traffic class, 4-bits of flow label
    uint16_t _flowLabel;      ///< Remaining 16-bits of 20-bit flow label
    uint16_t _length;         ///< Payload length (not including this header)
    uint8_t  _protocol;       ///< Identifies the type of header immediately following
    uint8_t  _hopLimit;       ///< Decremented by 1 by each node that forwards the packet
    IPv6Address _source;      ///< The IPv6 packet source address
    IPv6Address _destination; ///< The IPv6 packet destination address

}  __attribute__((__packed__));

/** The length of an Ethernet frame header */
#define ETHER_HEADER_LEN          (14)

/** The length of an IPv6 packet header */
#define IP6_HEADER_LEN            (40)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(IPv6Packet) == ETHER_HEADER_LEN + IP6_HEADER_LEN, "Size is not correct");


#endif
