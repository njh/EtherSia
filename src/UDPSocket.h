/** 
 * Header file for the UDPSocket class
 * @file UDPSocket.h
 */

#ifndef UDPSocket_H
#define UDPSocket_H

#include <stdint.h>
#include "IPv6Packet.h"

class EtherSia;

/**
 * Class for sending and receiving UDP packets on a specified port
 */
class UDPSocket {

public:

    /**
     * Construct a UDP socket
     *
     * @param ether The Ethernet interface to attach the socket to
     */
    UDPSocket(EtherSia *ether);

    /**
     * Construct a UDP socket, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the socket to
     * @param port The UDP port number to listen on
     */
    UDPSocket(EtherSia *ether, uint16_t port);

    /**
     * Construct a UDP socket, with a destination address and port
     *
     * @param ether The Ethernet interface to attach the socket to
     * @param destination The IPv6 address to send to
     * @param port The UDP port number to send to
     */
    UDPSocket(EtherSia *ether, IPv6Address *destination, uint16_t port);

    /**
     * Set the destination address and port to send packets to
     *
     * @param address The destination address as a human readable string
     * @param port The UDP port number to send packets to
     * @return true if the destination address was set successfully
     */
    boolean setDestination(const char *address, uint16_t port);

    /**
     * Set the destination address and port to send packets to
     *
     * @param address The destination address as a 16-byte array
     * @param port The UDP port number to send packets to
     * @return true if the destination address was set successfully
     */
    boolean setDestination(IPv6Address *address, uint16_t port);

    /**
     * Get the destination address that packets are being sent to
     * @return the IPv6 destination address
     */
    IPv6Address* getDestinationAddress();

    /**
     * Get the destination UDP port number that packets are being sent to
     * @return the port number
     */
    uint16_t getDestinationPort();

    /**
     * Check if a UDP packet is available to be processed on this socket
     * @return true if there is a valid packet has been received for this socket
     */
    boolean havePacket();

    /**
     * Verify the checksum of the UDP packet currently in the packet buffer
     * @return true if packet checksum is valid
     */
    boolean verifyChecksum();
    
    /**
     * Send a string out on the UDP socket
     * @param data The null-terminated string to send
     */
    void send(const char *data);

    /**
     * Send data out on the UDP socket
     * If data is NULL, it will send the data already in the EtherSia packet buffer
     *
     * @param data A pointer to the data to send
     * @param length The length (in bytes) of the data to send
     */
    void send(const uint8_t *data=NULL, uint16_t length=0);

    /**
     * Send a reply to the last packet received
     * @param data The null-terminated string to send
     */
    void sendReply(const char *data);

    /**
     * Send a reply to the last packet recieved
     * If data is NULL, it will send the data already in the EtherSia packet buffer
     *
     * @param data A pointer to the data to send
     * @param length The length (in bytes) of the data to send
     */
    void sendReply(const uint8_t *data=NULL, uint16_t length=0);

    /**
     * Get the IPv6 source address of the last UDP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The source IPv6 address
     */
    IPv6Address* packetSource();

    /**
     * Get the IPv6 destination address of the last UDP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The destination IPv6 address
     */
    IPv6Address* packetDestination();

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

    /**
     * Check if the current UDP payload equals a string
     *
     * @note Please call havePacket() first, before calling this method.
     * @param str The null-terminated string to compare to
     * @return True if the UDP payload is the same as the str parameter
     */
    boolean payloadEquals(const char *str);

protected:

    /**
     * Get a pointer to the current packet in the EtherSia buffer.
     *
     * @note This method does not check if the packet is valid or for this socket or not
     * @return A pointer to an IPv6Packet
     */
    IPv6Packet* getPacket();

    /**
     * Get the length of the current packet in the buffer (including the UDP header)
     *
     * @return The packet payload length (in bytes)
     */
    uint16_t packetLength();

    /**
     * Get the 16-bit checksum the current packet in the buffer
     *
     * @return The checksum of the packet
     */
    uint16_t packetChecksum();


    EtherSia *ether;            ///< The Ethernet Interface that this UDP socket is attached to
    IPv6Address destAddress;    ///< The IPv6 destination address
    uint16_t destPort;          ///< The IPv6 destination port number
};



/**
 * Structure for accessing the fields of a UDP packet header
 * @private
 */
struct udp_header {
    uint16_t srcPort;
    uint16_t destPort;
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
#define UDP_HEADER_PTR            ((struct udp_header*)(getPacket()->payload()))

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct udp_header) == UDP_HEADER_LEN, "Size is not correct");


#endif
