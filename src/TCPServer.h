/**
 * Header file for the TCPServer class
 * @file TCPServer.h
 */

#ifndef TCPServer_H
#define TCPServer_H

#include <stdint.h>
#include "IPv6Packet.h"

/**
 * Class for responding to TCP requests
 *
 * Requests and responses cannot be bigger than a single packet and
 * are limited by the size of the packet buffer.
 *
 * This class inherits from Print, so you you can also use the print()
 * and println() functions when composing a reply.
 *
 */
class TCPServer : public Print {

public:

    /**
     * Construct a TCP server, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the server to
     * @param localPort The local TCP port number to listen on
     */
    TCPServer(EtherSia &ether, uint16_t localPort);

    /**
     * Get the local TCP port number that packets are being sent to
     * @return the port number
     */
    uint16_t localPort();

    /**
     * Check if a TCP data packet is available to be processed for this server
     *
     * This method also has a side effect of responding to other stages
     * of the TCP sequence (the SYN and FIN packets).
     *
     * @return true if there is a valid packet with data has been received for this server
     */
    boolean havePacket();

    /**
     * Reply to a TCP request. The response should have been written to the
     * packet buffer using the print() and println() methods.
     */
    void sendReply();

    /**
     * Reply to a TCP request with a null-terminated string
     *
     * The string is copied into the packet buffer before sending.
     *
     * @param string The string to place in the reply
     */
    void sendReply(const char *string);

    /**
     * Reply to a TCP request with a buffer
     *
     * The bytes are copied into the packet buffer before sending.
     *
     * @param data The data to reply with
     * @param len The length of the data in bytes
     */
    void sendReply(const void* data, uint16_t len);

    /**
     * Get the IPv6 source address of the last TCP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The source IPv6 address
     */
    IPv6Address& packetSource();

    /**
     * Get the IPv6 destination address of the last TCP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The destination IPv6 address
     */
    IPv6Address& packetDestination();

    /**
     * Get the IPv6 source port number of the last TCP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The source port number
     */
    uint16_t packetSourcePort();

    /**
     * Get the IPv6 destination port number of the last TCP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The destination port number
     */
    uint16_t packetDestinationPort();

    /**
     * Get a pointer to the TCP payload of the current packet in the buffer
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    uint8_t* payload();

    /**
     * Get the length (in bytes) of the TCP payload of the packet
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    uint16_t payloadLength();

    /**
     * Check if the current TCP payload equals a string
     *
     * @note Please call havePacket() first, before calling this method.
     * @param str The null-terminated string to compare to
     * @return True if the TCP payload is the same as the str parameter
     */
    boolean payloadEquals(const char *str);

    /**
     * Write a single character into the TCP packet buffer
     *
     * @param chr The character to write
     * @return The number of bytes written to the buffer
     */
    virtual size_t write(uint8_t chr);

protected:
    enum TCP_FLAGS {
        TCP_FLAG_URG = 0x20,  ///< Urgent Pointer field is significant
        TCP_FLAG_ACK = 0x10,  ///< Acknowledgment field is significant
        TCP_FLAG_PSH = 0x08,  ///< Push function
        TCP_FLAG_RST = 0x04,  ///< Reset the connection
        TCP_FLAG_SYN = 0x02,  ///< Synchronise sequence numbers
        TCP_FLAG_FIN = 0x01   ///< No more data from sender
    };

    /**
     * Reply to a TCP packet, using data contained in buffer
     *
     * @param len The length of the data in the buffer
     * @param flags The flags to set on the outgoing packet (@ref TCP_FLAGS)
     * @return The number of bytes written to the buffer
     */
    void sendReplyWithFlags(uint16_t len, uint8_t flags);

    /** The Ethernet Interface that this TCP server is attached to */
    EtherSia &_ether;

    /** The TCP local port number */
    uint16_t _localPort;

    /** The current position of writing data to buffer (when using Print interface) */
    int16_t _responsePos;
};




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
 * The length of a TCP response packet header, used when sending packets
 * @private
 */
#define TCP_TRANSMIT_HEADER_LEN            (24)

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
