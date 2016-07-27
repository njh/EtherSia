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
     * The local port number will be sent to a random port number
     *
     * @param ether The Ethernet interface to attach the socket to
     */
    UDPSocket(EtherSia *ether);

    /**
     * Construct a UDP socket, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the socket to
     * @param localPort The local UDP port number to listen on
     */
    UDPSocket(EtherSia *ether, uint16_t localPort);

    /**
     * Construct a UDP socket, with a remote address and port
     *
     * @param ether The Ethernet interface to attach the socket to
     * @param remoteAddress The IPv6 address to send to
     * @param remotePort The UDP port number to send to
     */
    UDPSocket(EtherSia *ether, IPv6Address &remoteAddress, uint16_t remotePort);

    /**
     * Set the remote address (as a string) and port to send packets to
     *
     * If the remote address looks like a hostname, it will be looked up using DNS.
     *
     * @param remoteAddress The remote address or hostname
     * @param remotePort The UDP port number to send packets to
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(const char *remoteAddress, uint16_t remotePort);

    /**
     * Set the remote address and port to send packets to
     *
     * @param remoteAddress The remote address as a 16-byte array
     * @param remotePort The remote UDP port number to send packets to
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(IPv6Address &remoteAddress, uint16_t remotePort);

    /**
     * Get the remote address that packets are being sent to
     * @return the IPv6 remote address
     */
    IPv6Address& getRemoteAddress();

    /**
     * Get the remote UDP port number that packets are being sent to
     * @return the port number
     */
    uint16_t getRemotePort();

    /**
     * Check if a UDP packet is available to be processed on this socket
     * @return true if there is a valid packet has been received for this socket
     */
    boolean havePacket();

    /**
     * Send a string out on the UDP socket
     * @param data The null-terminated string to send
     */
    void send(const char *data);

    /**
     * Send the contents of the packet payload buffer
     *
     * Place the data in the payload() buffer before calling this method.
     *
     * @param length The length of the UDP payload
     */
    void send(uint16_t length);

    /**
     * Send data out on the UDP socket
     *
     * @param data A pointer to the data to send
     * @param length The length (in bytes) of the data to send
     */
    void send(const uint8_t *data, uint16_t length);

    /**
     * Send a reply to the last packet received
     * @param data The null-terminated string to send
     */
    void sendReply(const char *data);

    /**
     * Send a reply to the last packet received
     *
     * Place the data in the payload() buffer before calling this method.
     *
     * @param length The length (in bytes) of the data to send
     */
    void sendReply(uint16_t length);

    /**
     * Send a reply to the last packet recieved
     *
     * @param data A pointer to the data to send
     * @param length The length (in bytes) of the data to send
     */
    void sendReply(const uint8_t *data, uint16_t length);

    /**
     * Get the IPv6 source address of the last UDP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The source IPv6 address
     */
    IPv6Address& packetSource();

    /**
     * Get the IPv6 destination address of the last UDP packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The destination IPv6 address
     */
    IPv6Address& packetDestination();

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
    IPv6Address remoteAddress;  ///< The IPv6 remote address
    uint16_t remotePort;        ///< The IPv6 remote port number
    uint16_t localPort;         ///< The IPv6 local port number
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
#define UDP_HEADER_PTR            ((struct udp_header*)(getPacket()->payload()))

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(struct udp_header) == UDP_HEADER_LEN, "Size is not correct");


#endif
