/**
 * Header file for the TCPServer class
 * @file TCPServer.h
 */

#ifndef TCPServer_H
#define TCPServer_H

#include <stdint.h>
#include "IPv6Packet.h"
#include "Socket.h"

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
    enum TCP_FLAGS {
        TCP_FLAG_URG = 0x20,  ///< Urgent Pointer field is significant
        TCP_FLAG_ACK = 0x10,  ///< Acknowledgment field is significant
        TCP_FLAG_PSH = 0x08,  ///< Push function
        TCP_FLAG_RST = 0x04,  ///< Reset the connection
        TCP_FLAG_SYN = 0x02,  ///< Synchronise sequence numbers
        TCP_FLAG_FIN = 0x01   ///< No more data from sender
    };

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
    virtual void sendInternal(uint16_t length, boolean isReply);

    /**
     * Reply to a TCP packet, using data contained in buffer
     *
     * @param len The length of the data in the buffer
     * @param flags The flags to set on the outgoing packet (@ref TCP_FLAGS)
     * @return The number of bytes written to the buffer
     */
    void sendReplyWithFlags(uint16_t len, uint8_t flags);
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
