/**
 * Header file for the TCPServer class
 * @file TCPServer.h
 */

#ifndef TCPServer_H
#define TCPServer_H

#include <stdint.h>
#include "IPv6Packet.h"
#include "Socket.h"
#include "tcp.h"

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
class TCPServer : public Socket {

public:

    /**
     * Construct a TCP server, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the server to
     * @param localPort The local TCP port number to listen on
     */
    TCPServer(EtherSia &ether, uint16_t localPort);

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
     * Get a pointer to the TCP payload of the last received packet
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    virtual uint8_t* payload();

    /**
     * Get the length (in bytes) of the last received TCP packet payload
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    virtual uint16_t payloadLength();

    /**
     * Get a pointer to the next TCP packet payload to be sent
     *
     * @return A pointer to the transmit payload buffer
     */
    virtual uint8_t* transmitPayload();

protected:

    /**
     * Internal function to send a TCP packet, using data contained in buffer
     *
     * @param length The length of the data in the buffer
     */
    virtual void sendInternal(uint16_t length, boolean isReply);

};


#endif
