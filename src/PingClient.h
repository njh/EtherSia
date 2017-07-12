/**
 * Header file for the PingClient class
 * @file PingClient.h
 */

#ifndef PingClient_H
#define PingClient_H

#include <stdint.h>
#include "IPv6Packet.h"
#include "Socket.h"

class EtherSia;

/**
 * Class for sending and receiving ICMPv6 Ping packets
 */
class PingClient: public Socket {

public:

    /**
     * Construct a Ping Client
     *
     * @param ether The Ethernet interface to send pings from
     */
    PingClient(EtherSia &ether);

    /**
     * Set the remote address to send the pings to
     *
     * @param remoteAddress The remote address or hostname
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(const char *remoteAddress);

    /**
     * Check if a Ping packet is has been received on the Ethernet interface
     * @return true if there is a valid ping packet has been received
     */
    boolean havePacket();

    /**
     * Send an ICMPv6 Echo packet to the remote address
     *
     * If there is nothing in the payload buffer, 8 random bytes are
     * written before sending the packet.
     */
    void send();

    /**
     * Get a pointer to the ICMPv6 Echo payload of the current packet in the buffer
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    uint8_t* payload();

    /**
     * Get the length (in bytes) of the ICMPv6 Echo payload of the packet
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    uint16_t payloadLength();

    /**
     * Get the ICMPv6 Echo identifier to be used for packets sent by this client
     *
     * @return The ICMPv6 Echo identifier
     */
    uint16_t identifier() { return _identifier; }
    
    /**
     * Set the ICMPv6 Echo identifier to be used in sent packets
     *
     * @param identifier The ICMPv6 Echo identifier to use
     */
    void setIdentifer(uint16_t identifier) { _identifier = identifier; }
    
    /**
     * Get the ICMPv6 Echo sequence number for the next packet to be sent
     *
     * @return The ICMPv6 Echo identifier
     */
    uint16_t sequenceNumber() { return _sequenceNumber; }
    

     /**
      * Check if we received a reply for the last echo packet sent
      *
      * @return True if we have received a reply for the last echo sent
      */
    boolean gotReply() { return _gotReply; }
    /**
     * Set the ICMPv6 Echo sequence number for the next packet to be sent
     *
     * @param seqNum The ICMPv6 Echo identifier
     */
    void setSequenceNumber(uint16_t seqNum) { _sequenceNumber = seqNum; }

protected:

    uint16_t _identifier;
    uint16_t _sequenceNumber;

    boolean _gotReply;          /**< True if we have received a reply for the last echo sent */

    /**
     * Send a ICMPv6 Ping packet
     *
     * @param length The length (in bytes) of the data to send
     */
    void sendInternal(uint16_t length, boolean isReply);
};


#endif
