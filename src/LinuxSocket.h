/**
 * Header file for using EtherSia with a network Socket on Linux
 * @file LinuxSocket.h
 */

#ifndef LINUXSOCKET_H
#define LINUXSOCKET_H

#include <net/if.h>

#include "EtherSia.h"

/**
 * Run EtherSia on Linux using a raw socket to Send and receive Ethernet frames
 * Not intended for use with running EtherSia on Arduino.
 *
 * @note this is probably only useful for testing and development of EtherSia.
 */
class EtherSia_LinuxSocket : public EtherSia {

public:
    /**
     * Constructor
     * @param iface the name of the Ethernet interface to send/receive on
     */
    EtherSia_LinuxSocket(const char* iface = NULL);

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

protected:

    char ifname[IFNAMSIZ];
    int ifindex;
    int sockfd;
};

#endif /* LINUXSOCKET_H */
