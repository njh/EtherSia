/**
 * Header file for the UDPSocket class
 * @file UDPSocket.h
 */

#ifndef UDPSocket_H
#define UDPSocket_H

#include <stdint.h>
#include "IPv6Packet.h"
#include "Socket.h"

class EtherSia;

/**
 * Class for sending and receiving UDP packets on a specified port
 */
class UDPSocket: public Socket {

public:

    /**
     * Construct a UDP socket
     * The local port number will be sent to a random port number
     *
     * @param ether The Ethernet interface to attach the socket to
     */
    UDPSocket(EtherSia &ether);

    /**
     * Construct a UDP socket, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the socket to
     * @param localPort The local UDP port number to listen on
     */
    UDPSocket(EtherSia &ether, uint16_t localPort);

    /**
     * Check if a UDP packet is available to be processed on this socket
     * @return true if there is a valid packet has been received for this socket
     */
    boolean havePacket();

    /**
     * Get the IPv6 source port number of the last UDP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The source port number
     */
    uint16_t packetSourcePort();

    /**
     * Get the IPv6 destination port number of the last UDP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The destination port number
     */
    uint16_t packetDestinationPort();

    /**
     * Get a pointer to the UDP payload of the current packet in the buffer
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    uint8_t* payload();

    /**
     * Get the length (in bytes) of the UDP payload of the packet
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    uint16_t payloadLength();

protected:

    /**
     * Send a UDP packet, based on the contents of the buffer.
     * This function:
     * - sets the IP protocol number
     * - sets the IP packet length
     * - sets the UDP packet length
     * - sets the UDP source port number
     * - sets the UDP distination port number
     * - sets the UDP checksum
     *
     * @param length The length (in bytes) of the data to send
     */
    void sendInternal(uint16_t length, boolean isReply);
};



/**
 * Structure for accessing the fields of a UDP packet header
 * @private
 */
struct udp_header {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;
} __attribute__((__packed__));

/**
 * The length of a UDP packet header
 * @private
 */
#define UDP_HEADER_LEN            (8)

/**
 * Get the pointer to the UDP header from within EtherSia
 * @private
 */
#define UDP_HEADER_PTR            ((struct udp_header*)(packet.payload()))

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct udp_header) == UDP_HEADER_LEN, "Size is not correct");


#endif
