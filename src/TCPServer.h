/**
 * Header file for the TCPServer class
 * @file TCPServer.h
 */

#ifndef TCPServer_H
#define TCPServer_H

#include <stdint.h>
#include "IPv6Packet.h"

class EtherSia;

/**
 * Class for sending and receiving TCP packets on a specified port
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
     * Check if a TCP packet is available to be processed on this server
     * @return true if there is a valid packet has been received for this server
     */
    boolean havePacket();



    void sendReply();

    void sendReply(const char *string);

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
    uint8_t* requestPayload();

    /**
     * Get the length (in bytes) of the TCP payload of the packet
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    uint16_t requestLength();

    /**
     * Check if the current TCP payload equals a string
     *
     * @note Please call havePacket() first, before calling this method.
     * @param str The null-terminated string to compare to
     * @return True if the TCP payload is the same as the str parameter
     */
    boolean requestEquals(const char *str);

    /**
     * Write a single character into the TCP packet buffer
     *
     * @param chr The character to write
     * @return The number of bytes written to the buffer
     */
    virtual size_t write(uint8_t chr);

protected:

    void sendReplyWithFlags(uint16_t len, uint8_t flags);

    EtherSia &_ether;            ///< The Ethernet Interface that this TCP server is attached to
    uint16_t _localPort;         ///< The IPv6 local port number

    //IPv6Address _remoteAddress;  ///< The IPv6 remote address
    //uint16_t _remotePort;        ///< The IPv6 remote port number

    int16_t _responsePos;
};


#define TCP_FLAG_URG   (0x20)
#define TCP_FLAG_ACK   (0x10)
#define TCP_FLAG_PSH   (0x08)
#define TCP_FLAG_RST   (0x04)
#define TCP_FLAG_SYN   (0x02)
#define TCP_FLAG_FIN   (0x01)

enum TCP_STATES {
  TCP_LISTEN,
  TCP_SYN_RECEIVED,
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
    uint8_t mssOptionKind;
    uint8_t mssOptionLen;
    uint16_t mssOptionValue;
} __attribute__((__packed__));

/**
 * The length of a TCP packet header
 * @private
 */
#define TCP_HEADER_LEN            (24)

#define TCP_WINDOW_SIZE           (ETHERSIA_MAX_PACKET_SIZE - \
                                   ETHER_HEADER_LEN - IP6_HEADER_LEN - TCP_HEADER_LEN - 32)

//#define TCP_WINDOW_SIZE           (1024)

/**
 * Get the pointer to the TCP header from within EtherSia
 * @private
 */
#define TCP_HEADER_PTR            ((struct tcp_header*)(packet.payload()))

/**
 * Get the length of the TCP header in bytes
 * @private
 */
#define TCP_DATA_OFFSET           ((TCP_HEADER_PTR->dataOffset & 0xF0) >> 2)

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct tcp_header) == TCP_HEADER_LEN, "Size is not correct");


#endif
