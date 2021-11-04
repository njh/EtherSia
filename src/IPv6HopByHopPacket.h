/**
 * Header file for the ICMPv6Packet class
 * @file IPv6HopByHopPacket.h
 */
#include "util.h"

#ifndef IPV6_HOP_BY_HOP_PACKET_H
#define IPV6_HOP_BY_HOP_PACKET_H

/** Field-Type Value of a Router Alert option */
#define IPV6_HBH_OPTION_ROUTER_ALERT 5

/** Length of a Multicast Listener Discovery Hop-by-Hop option **/
#define IPV6_HBH_MLD_OPTION_LENGTH 8

/**
 * Structure representing a Router Alert option
 * @private
 */
struct router_alert {
    uint8_t option_type;
    uint8_t length;
    uint16_t alert_type;
    uint16_t padding;
} __attribute__((__packed__));

/**
 * Class for getting and setting the fields of an IPv6 Hop-by-Hop Packet
 * with Hop-by-Hop extension and its Ethernet header
 */
class IPv6HopByHopPacket : public IPv6Packet {
    public:
        /**
         * Constructor for new empty IPv6 Hop-by-Hop packet
         */
         IPv6HopByHopPacket();

        /**
         * Set the type of the Hop by Hop packet payload
         * @param header typecode of the payload type
         */
        void setNextHeader(uint8_t header);

        /**
         * Return the type of the Hop by Hop packet payload
         * @return typecode of the payload type
         */
        uint8_t nextHeader();

        /**
         * Set the Hop by Hop option length
         * @param length hop by hop option length
         */
        void setHopByHopLength(uint8_t length);

        /**
         * Return the Hop by Hop option length
         * @return hop by hop option length
         */
        uint8_t hopByHopLength();

        /**
         * Return the Hop by Hop option type
         * @return hop by hop option type
         */
        uint8_t optionType();

        /**
         * Set Hop-by-Hop option to Routar Alert with
         * MLD alert-type
         */
        void setMLDRouterAlert();

        /**
         * Calculate the 16-bit checksum for the IPv6 packet
         * @return the checksum of the packet
         */
        uint16_t calculateChecksum();

        /**
         * Get a pointer to the start of the IPv6 Hop-by-Hop packet payload
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

    protected:
        uint8_t _next_header;   ///< The Type of the packet payload
        uint8_t _hbh_length;    ///< The hop-by-hop option length
        union {
            uint8_t hbh_option[6];              ///< Raw hop-by-hop option content
            struct router_alert _router_alert;   ///< RouterAlert hop-by-hop option
        };
} __attribute__((__packed__));

#endif