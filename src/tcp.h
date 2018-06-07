/**
 * Header file for the TCP data structures
 * @file tcp.h
 */

#ifndef TCP_H
#define TCP_H

#include <stdint.h>


/**
 * Structure for accessing the fields of a TCP packet header
 * @private
 */
struct tcp_header {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint32_t sequenceNum;
    uint32_t acknowledgementNum;
    uint8_t dataOffset;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgentPointer;

    // Following fields are options used when sending/transmitting only
    uint8_t mssOptionKind;
    uint8_t mssOptionLen;
    uint16_t mssOptionValue;
} __attribute__((__packed__));

/**
 * Enumeration for the values of tcp_header.flags
 * @private
 */
enum TCP_FLAGS {
    TCP_FLAG_URG = 0x20,  ///< Urgent Pointer field is significant
    TCP_FLAG_ACK = 0x10,  ///< Acknowledgment field is significant
    TCP_FLAG_PSH = 0x08,  ///< Push function
    TCP_FLAG_RST = 0x04,  ///< Reset the connection
    TCP_FLAG_SYN = 0x02,  ///< Synchronise sequence numbers
    TCP_FLAG_FIN = 0x01   ///< No more data from sender
};

/**
 * The minimum length of a TCP response packet without any extra options
 * @private
 */
#define TCP_MINIMUM_HEADER_LEN             (20)

/**
 * The length of a TCP response packet header, used when sending packets
 * @private
 */
#define TCP_TRANSMIT_HEADER_LEN            (TCP_MINIMUM_HEADER_LEN + 4)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct tcp_header) == TCP_TRANSMIT_HEADER_LEN, "Size is not correct");

/**
 * Get the length of the recieved TCP header in bytes
 * @private
 */
#define TCP_RECEIVE_HEADER_LEN    ((TCP_HEADER_PTR->dataOffset & 0xF0) >> 2)

/**
 * The maximum size of the TCP segment that we can receive
 * @private
 */
#define TCP_WINDOW_SIZE           (ETHERSIA_MAX_PACKET_SIZE - \
                                   ETHER_HEADER_LEN - IP6_HEADER_LEN - TCP_TRANSMIT_HEADER_LEN - 32)

/**
 * Get the pointer to the TCP header from within EtherSia
 * @private
 */
#define TCP_HEADER_PTR            ((struct tcp_header*)(packet.payload()))


#endif
