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
     * Construct a UDP socket
     * The local port number will be sent to a random port number
     *
     * @param ether The Ethernet interface to attach the socket to
     */
    Socket(EtherSia &ether);

    /**
     * Construct a UDP socket, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the socket to
     * @param localPort The local UDP port number to listen on
     */
    Socket(EtherSia &ether, uint16_t localPort);

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
    IPv6Address& remoteAddress();

    /**
     * Get the remote UDP port number that packets are being sent to
     * @return the port number
     */
    uint16_t remotePort();

    /**
     * Get the local UDP port number that packets are being sent to
     * @return the port number
     */
    uint16_t localPort();

    /**
     * Check if a UDP packet is available to be processed on this socket
     * @return true if there is a valid packet has been received for this socket
     */
    boolean havePacket();


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

protected:

    EtherSia &_ether;            ///< The Ethernet Interface that this UDP socket is attached to
    IPv6Address _remoteAddress;  ///< The IPv6 remote address
    MACAddress _remoteMac;       ///< The Ethernet address to send packets to
    uint16_t _remotePort;        ///< The UDP remote port number
    uint16_t _localPort;         ///< The UDP local port number
};



#endif
