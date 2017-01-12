/**
 * Header file for using EtherSia with a dummy Ethernet interface
 * @file LinuxSocket.h
 */

#ifndef DUMMY_H
#define DUMMY_H

#include "EtherSia.h"

#include <time.h>

/**
 * Structure for storing information about sent and received packets
 *
 * Only used for testing with EtherSia_Dummy
 *
 * @private
 */
typedef struct frame_wrapper {
    time_t time;         ///< A UNIX timestamp for the time the packet was sent/received
    uint16_t length;     ///< The length of the packet (in bytes)
    IPv6Packet* packet;  ///< A pointer to the packet data
} frame_t;

/**
 * A dummy Ethernet interface, that can be useful for testing
 *
 * @note this is probably only useful for the testing and development of EtherSia.
 */
class EtherSia_Dummy : public EtherSia {

public:
    /**
     * Create a new dummy ethernet controller
     */
    EtherSia_Dummy();

    // Tell the compiler we want to use begin() from the base class
    using EtherSia::begin;

    /**
     * Initialise the Ethernet controller
     * Must be called before sending or receiving Ethernet frames
     *
     * @param address the local MAC address for the Ethernet interface
     * @return Returns true if setting up the Ethernet interface was successful
     */
    virtual boolean begin(const MACAddress &address);

    /**
     * Send an Ethernet frame
     * @param data a pointer to the data to send
     * @param datalen the length of the data in the packet
     * @return the number of bytes transmitted
     */
    virtual uint16_t sendFrame(const uint8_t *data, uint16_t datalen);

    /**
     * Read an Ethernet frame
     * @param buffer a pointer to a buffer to write the packet to
     * @param bufsize the available space in the buffer
     * @return the length of the received packet
     *         or 0 if no packet was received
     */
    virtual uint16_t readFrame(uint8_t *buffer, uint16_t bufsize);

    /**
     * Close the dummy ethernet socket
     *
     * Frees up memory used by packet buffers
     */
    virtual void end();

    /**
     * Add a packet into the buffer, to be received by EtherSia
     *
     * @param packet pointer to the packet to add to the receive list
     * @param length the length of the packet (in bytes)
     */
    void injectRecievedPacket(void *packet, uint16_t length);

    /**
     * Clear the list of received packets
     */
    void clearRecieved();

    /**
     * Clear the list of sent packets
     */
    void clearSent();

    /**
     * Get a packet sent by EtherSia
     *
     * @param pos the packet number to return (by default the first)
     * @return A structure containing a pointer to the packet and its length
     */
    frame_t& getSent(size_t pos=0);

    /**
     * Get the most recently sent packet by EtherSia
     * @return A structure containing a pointer to the packet and its length
     */
    frame_t& getLastSent();

    /**
     * Get number of packets that EtherSia has received
     * This includes invalid or ignored packets
     * @return the number of packets received
     */
    size_t getRecievedCount() {
        return _recievedCount;
    }

    /**
     * Get number of packets made available to EtherSia to receive
     * @return the number of packets injected
     */
    size_t getInjectCount() {
        return _injectCount;
    }

    /**
     * Get number of packets that EtherSia has sent
     * @return the number of packets sent
     */
    size_t getSentCount() {
        return _sentCount;
    }

protected:
    static const size_t bufferSize = 100;

    frame_t _recieved[bufferSize];
    size_t _injectCount;
    size_t _recievedCount;
    frame_t _sent[bufferSize];
    size_t _sentCount;

};


#endif /* DUMMY_H */
