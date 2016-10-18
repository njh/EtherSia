/**
 * Header file for using EtherSia with a dummy Ethernet interface
 * @file LinuxSocket.h
 */

#ifndef DUMMY_H
#define DUMMY_H

#include "EtherSia.h"

#include <time.h>

typedef struct frame_wrapper {
    time_t time;
    size_t length;
    IPv6Packet* packet;
} frame_t;

/**
 * A dummy Ethernet interface, that can be useful for testing
 *
 * @note this is probably only useful for testing and development of EtherSia.
 */
class EtherSia_Dummy : public EtherSia {

public:
    /**
     * Constructor
     * @param iface the name of the Ethernet interface to send/receive on
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
     * Close the raw ethernet socket
     */
    virtual void end();


    void injectRecievedPacket(IPv6Packet &packet, size_t len=0);

    frame_t& getSent(size_t pos=0);
    frame_t& getLastSent();

    size_t getRecievedCount() {
        return _recievedCount;
    }
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
