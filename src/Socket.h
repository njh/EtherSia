/**
 * Header file for the Socket class
 * @file Socket.h
 */

#ifndef Socket_H
#define Socket_H

#include <stdint.h>
#include "IPv6Packet.h"

class EtherSia;

/**
 * Abstract base class for a IP socket
 */
class Socket {

public:

    /**
     * Construct a socket
     * The local port number will be sent to a random port number
     *
     * @param ether The Ethernet interface to attach the socket to
     */
    Socket(EtherSia &ether);

    /**
     * Construct a socket, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the socket to
     * @param localPort The local port number to listen on
     */
    Socket(EtherSia &ether, uint16_t localPort);

    /**
     * Set the remote address (as a string) and port to send packets to
     *
     * If the remote address looks like a hostname, it will be looked up using DNS.
     *
     * @param remoteAddress The remote address or hostname
     * @param remotePort The port number to send packets to
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(const char *remoteAddress, uint16_t remotePort);

    /**
     * Set the remote address and port to send packets to
     *
     * @param remoteAddress The remote address as a 16-byte array
     * @param remotePort The remote port number to send packets to
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(IPv6Address &remoteAddress, uint16_t remotePort);

    /**
     * Get the remote address that packets are being sent to
     * @return the IPv6 remote address
     */
    IPv6Address& remoteAddress();

    /**
     * Get the remote port number that packets are being sent to
     * @return the port number
     */
    uint16_t remotePort();

    /**
     * Get the local port number that packets are being sent to
     * @return the port number
     */
    uint16_t localPort();

    /**
     * Get the IPv6 source address of the last packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The source IPv6 address
     */
    IPv6Address& packetSource();

    /**
     * Get the IPv6 destination address of the last packet received
     *
     * @note Please call havePacket() first, before calling this method.
     * @return The destination IPv6 address
     */
    IPv6Address& packetDestination();

    /**
     * Send the contents of the packet payload buffer
     *
     * Place the data in the payload() buffer before calling this method.
     *
     * @param length The length of the payload
     */
    void send(uint16_t length);

    /**
     * Send a packet containing a string from socket
     *
     * @param data The null-terminated string to send
     */
    void send(const char *data);

    /**
     * Send a packet containing an array of bytes from socket
     *
     * @param data The data to send as the payload
     * @param length The length (in bytes) of the data to send
     */
    void send(const void *data, uint16_t length);

    /**
     * Send a reply to the last packet received
     *
     * Place the data in the payload() buffer before calling this method.
     *
     * @param length The length (in bytes) of the data to send
     */
    void sendReply(uint16_t length);

    /**
     * Send a reply to the last packet received
     * @param data The null-terminated string to send
     */
    void sendReply(const char *data);

    /**
     * Send a reply to the last packet received
     *
     * @param data A pointer to the data to send
     * @param length The length (in bytes) of the data to send
     */
    void sendReply(const void *data, uint16_t length);

    /**
     * Get a pointer to the payload of the current packet in the buffer
     *
     * @note This method must be implemented by sub-classes
     * @return A pointer to the payload
     */
    virtual uint8_t* payload() = 0;

    /**
     * Get the length (in bytes) of the payload of the packet
     *
     * @note This method must be implemented by sub-classes
     * @return A pointer to the payload
     */
    virtual uint16_t payloadLength() = 0;

protected:
    virtual void sendInternal(uint16_t length, boolean isReply) = 0;

    EtherSia &_ether;            ///< The Ethernet Interface that this socket is attached to
    IPv6Address _remoteAddress;  ///< The IPv6 remote address
    MACAddress _remoteMac;       ///< The Ethernet address to send packets to
    uint16_t _remotePort;        ///< The remote port number
    uint16_t _localPort;         ///< The local port number
};



#endif
