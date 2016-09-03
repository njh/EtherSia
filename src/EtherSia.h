/**
 * Main header file for EtherSia - include this in your project
 * @file EtherSia.h
 */

#ifndef EtherSia_H
#define EtherSia_H

#include <Arduino.h>
#include <stdint.h>

#include "endian.h"
#include "MACAddress.h"
#include "IPv6Address.h"
#include "IPv6Packet.h"
#include "UDPSocket.h"
#include "Syslog.h"

/**
 * The maximum size (in bytes) of packet that can be received / sent
 *
 * This includes the Ethernet frame header and IPv6 header (54 bytes).
 * The maximum Ethernet frame size (MTU/MRU) is typically 1500 bytes.
 *
 * The value is used to size the buffer that is used for both
 * sending and receiving packets, so it should be bigger than the
 * biggest packet you want to send or receive.
 */
#define ETHERSIA_MAX_PACKET_SIZE       500



/** How often to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_TIMEOUT    (3000)

/** How many times to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_ATTEMPTS   (4)


/**
 * Main class for sending and receiving IPv6 messages using the ENC28J60 Ethernet controller
 *
 * Create an instance of this class for talking to your Ethernet Controller
 */
class EtherSia {
public:
    /**
     * Default Constructor
     */
    EtherSia();

    /**
     * Manually set the global IPv6 address for the Ethernet Interface
     * from an IPv6Address object
     *
     * This is not needed, if stateless auto-configuration is used.
     *
     * @note Call this before begin().
     * @param address The Global IP address for this Ethernet interface
     */
    void setGlobalAddress(IPv6Address &address);

    /**
     * Manually set the global IPv6 address for the Ethernet Interface
     * from human readable string.
     *
     * @note Call this before begin().
     * @param address The Global IP address for this Ethernet interface
     */
    void setGlobalAddress(const char* address);

    /**
     * Get the global IPv6 address of the Ethernet Interface
     * If this is called after begin(), then it will return
     * the IP address assigned during stateless auto-configuration.
     *
     * @return The Global IP address as an IPv6Address object
     */
    IPv6Address& globalAddress();

    /**
     * Get the link-local address of the Ethernet Interface
     * This is generated automatically from the MAC address.
     *
     * @return The Link-Local IP address as an IPv6Address object
     */
    IPv6Address& linkLocalAddress();

    /**
     * Check to see if an IPv6 address belongs to this Ethernet interface
     *
     * @param address the IPv6Addrss to check
     * @return ture if the
     */
    uint8_t isOurAddress(const IPv6Address &address);

    /**
     * Set the IPv6 address DNS server to use for hostname lookups
     *
     * @param address The DNS Server IP address
     */
    void setDnsServerAddress(IPv6Address &address);

    /**
     * Get the IPv6 address of the DNS server
     *
     * @return The DNS Server address as an IPv6Address object
     */
    IPv6Address& dnsServerAddress();

    /**
     * Check if there is an IPv6 packet waiting for us and copy it into the buffer.
     * If there is no packet available this method returns 0.
     *
     * @return The length of the packet, or 0 if no packet was received
     */
    uint16_t receivePacket();

    /**
     * Get a reference to the packet buffer (the last packet sent or received).
     *
     * There is a single buffer that is use for both sending and receiving packets,
     * so the packet returned may be in either state.
     *
     * It is also possible that it is an invalid or uninitialised packet.
     *
     * @return A reference to an IPv6Packet
     */
    IPv6Packet& packet();

    /**
     * Send the packet currently in the packet buffer.
     */
    void send();

    /**
     * Get the packet buffer ready to send a packet
     *
     * This method sets:
     * - EtherType, IP version, Traffic Class, Flow Label, Hop Limit
     * - Ethernet and IPv6 source address
     * - Ethernet Destination address
     *
     * It should be called after setting the IPv6 source and destination addresses
     */
    void prepareSend();

    /**
     * Convert the packet current in the packet buffer into a reply
     *
     * This method sets:
     * - EtherType, IP version, Traffic Class, Flow Label, Hop Limit
     * - Sets the IPv6 destination and to the source address
     * - Sets the IPv6 source address to either our link local or global address
     * - Swaps the Ethernet source and destination addresses
     *
     */
    void prepareReply();

    /**
     * Lookup a hostname using DNS and get an IPv6 address for it
     *
     * This method receives and processes UDP packets while it is
     * waiting for the DNS reply. Other packets may be missed / lost
     * while this method is running.
     *
     * It is recommended that this method is called within setup(),
     * to avoid packets being lost within loop().
     *
     * @note You probably don't need to call this function directly.
     * @param hostname The hostname to look up
     * @return An pointer to a IPv6 address or NULL if the lookup failed
     */
    IPv6Address* lookupHostname(const char* hostname);

    /**
     * Configure the Ethernet interface and get things ready
     *
     * If a IPv6 address has not already been set, then
     * stateless auto-configuration will start - attempting to
     * get an IP address and Router address using IGMP6.
     *
     * @return Returns true if setting up the Ethernet interface was successful
     */
    virtual boolean begin();

    /**
     * Send an Ethernet frame
     * @param data a pointer to the data to send
     * @param datalen the length of the data in the packet
     * @return the number of bytes transmitted
     */
    virtual uint16_t sendFrame(const uint8_t *data, uint16_t datalen) = 0;

    /**
     * Read an Ethernet frame
     * @param buffer a pointer to a buffer to write the packet to
     * @param bufsize the available space in the buffer
     * @return the length of the received packet
     *         or 0 if no packet was received
     */
    virtual uint16_t readFrame(uint8_t *buffer, uint16_t bufsize) = 0;

protected:
    IPv6Address _linkLocalAddress;  /**< The IPv6 Link-local address of the Ethernet Interface */
    IPv6Address _globalAddress;     /**< The IPv6 Global address of the Ethernet Interface */
    IPv6Address _dnsServerAddress;  /**< The IPv6 address of the configured DNS server */

    /** The MAC address of this Ethernet controller */
    MACAddress _localMac;

    /** The MAC Address of the router to send packets outside of this subnet */
    MACAddress _routerMac;

    /** The buffer that sent and received packets are stored in */
    uint8_t _buffer[ETHERSIA_MAX_PACKET_SIZE];


    /**
     * Process a received ICMPv6 packet in the packet buffer
     *
     * Responds to ICMPv6 Pings, Neighbour Solicitations and Router Solicitations
     */
    void icmp6ProcessPacket();

    /**
     * Perform Stateless auto-configuration
     *
     * @return true if successful, false if failure
     */
    boolean icmp6AutoConfigure();

    /**
     * Send a ICMPv6 Neighbour Solicitation (NS) for specified IPv6 Address
     *
     * @param targetAddress The IPv6 address to be resolved
     */
    void icmp6SendNS(IPv6Address &targetAddress);

    /**
     * Send a reply to a ICMPv6 Neighbour Solicitation (NS) request (if it is our address)
     */
    void icmp6NSReply();

    /**
     * Send a reply to a ICMPv6 Echo request
     */
    void icmp6EchoReply();

    /**
     * Send a reply to a ICMPv6 Router Solicitation (RS) request
     */
    void icmp6SendRS();

    /**
     * Handle a ICMPv6 Router Advertisement (RA) packet
     *
     * If it is valid, the router MAC and our Global address will be set
     */
    void icmp6ProcessRA();

    /**
     * Handle a single Prefix from a Router Advertisement (RA) packet
     */
    void icmp6ProcessPrefix(struct icmp6_prefix_information *pi);

    /**
     * Send an ICMPv6 packet stored in the EtherSia packet buffer
     * Ensures the protocol and checksum are set before sending.
     */
    void icmp6PacketSend();
};


#include "enc28j60.h"
#include "w5100.h"

#ifndef ARDUINO
#include "LinuxSocket.h"
#endif


#endif
