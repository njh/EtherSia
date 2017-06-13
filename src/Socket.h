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
class Socket : public Print {

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
     * Set the remote address (as a string in flash memory) and port to send packets to
     *
     * If the remote address looks like a hostname, it will be looked up using DNS.
     *
     * @param remoteAddress The remote address or hostname (as a flash string - use the F() macro)
     * @param remotePort The port number to send packets to
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(const __FlashStringHelper* remoteAddress, uint16_t remotePort);

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
     * Send out a packet
     *
     * Before calling this, the payload should have been written to the
     * packet buffer using the print() and println() methods.
     *
     * @param isReply true if the sent packet is a reply to the packet current in the buffer
     */
    void send(boolean isReply=false);

    /**
     * Send the contents of the packet payload buffer
     *
     * Place the data in the payload() buffer before calling this method.
     *
     * @param length The length of the payload
     * @param isReply true if the sent packet is a reply to the packet current in the buffer
     */
    void send(uint16_t length, boolean isReply=false);

    /**
     * Send a packet containing a string from socket
     *
     * @param data The null-terminated string to send
     * @param isReply true if the sent packet is a reply to the packet current in the buffer
     */
    void send(const char *data, boolean isReply=false);

    /**
     * Send a packet containing an array of bytes from socket
     *
     * @param data The data to send as the payload
     * @param length The length (in bytes) of the data to send
     * @param isReply true if the sent packet is a reply to the packet current in the buffer
     */
    void send(const void *data, uint16_t length, boolean isReply=false);

    /**
     * Send a reply to an incoming packet
     *
     * Before calling this, the payload should have been written to the
     * packet buffer using the print() and println() methods.
     */
    void sendReply();

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

    /**
     * Check if the current payload equals a string
     *
     * @note Please call havePacket() first, before calling this method.
     * @param str The null-terminated string to compare to
     * @return True if the UDP payload is the same as the str parameter
     */
    boolean payloadEquals(const char *str);

    /**
     * Get a pointer to the next packet payload to be sent
     *
     * @return A pointer to the transmit payload buffer
     */
    virtual uint8_t* transmitPayload();

    /**
     * Write a single character into the packet buffer
     *
     * @param chr The character to write
     * @return The number of bytes written to the buffer
     */
    virtual size_t write(uint8_t chr);

protected:

    /**
     * This method is called when a newline is written using print()
     *
     * Default behaviour is to handle Carriage Return or Line Feed
     * in the normal way, like any other character.
     *
     * But some sub-classes my want to overload it and use it as a
     * trigger to send a packet.
     *
     * @return True if the newline character should be written to the buffer
     */
    virtual boolean handleWriteNewline();

    /**
     * This method is called when it is time to initialise a new packet
     *
     * Default behaviour is to do nothing.
     *
     * But some sub-classes my want to use it write a header into the
     * buffer before appending other character are written using print().
     */
    virtual void writePayloadHeader();

    /**
     * Protocol specific function that is called by send(), sendReply() etc.
     *
     * @note must be overloaded by sub-classes
     * @param length The length (in bytes) of the data to send
     * @param isReply Set to true if this packet is a reply to an incoming packet
     */
    virtual void sendInternal(uint16_t length, boolean isReply) = 0;

    EtherSia &_ether;            ///< The Ethernet Interface that this socket is attached to
    IPv6Address _remoteAddress;  ///< The IPv6 remote address
    MACAddress _remoteMac;       ///< The Ethernet address to send packets to
    uint16_t _remotePort;        ///< The remote port number
    uint16_t _localPort;         ///< The local port number

    /** The current position of writing data to buffer (when using Print interface) */
    int16_t _writePos;
};



#endif
