/**
 * Main header file for EtherSia - include this in your project
 * @file EtherSia.h
 */

#ifndef EtherSia_H
#define EtherSia_H

#include <Arduino.h>
#include <stdint.h>

#include "enc28j60.h"
#include "MACAddress.h"
#include "IPv6Address.h"
#include "IPv6Packet.h"
#include "UDPSocket.h"

/** Convert a 16-bit integer from host (little-endian) to network (big-endian) */
#ifndef htons
#define htons(x) ( (((x)<<8)&0xFF00) | (((x)>>8)&0x00FF) )
#endif

/** Convert a 16-bit integer from network (big-endian) to host (little-endian) */
#ifndef ntohs
#define ntohs(x) htons(x)
#endif

/** How often to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_TIMEOUT    (3000)

/** How many times to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_ATTEMPTS   (4)


/**
 * Main class for sending and receiving IPv6 messages using the ENC28J60 Ethernet controller
 *
 * Create an instance of this class for talking to your Ethernet Controller
 */
class EtherSia : public ENC28J60 {
public:
    /**
     * Constructor that uses the default hardware SPI pins
     * @param cs the Arduino Chip Select / Slave Select pin (default 10)
     */
    EtherSia(int8_t cs=10);

    /**
     * Constructor for using software SPI, with custom set of pins
     * @param clk the SPI Clock pin
     * @param miso the SPI Master In / Slave Out pin
     * @param mosi the SPI Master Out / Slave In pin
     * @param cs the Arduino Chip Select / Slave Select pin
     */
    EtherSia(int8_t clk, int8_t miso, int8_t mosi, int8_t cs);

    /**
     * Configure the Ethernet interface and get things ready
     *
     * If a IPv6 address has not already been set, then
     * stateless auto-configuration will start - attempting to
     * get an IP address and Router address using IGMP6.
     *
     * @param address The local MAC address for the Ethernet interface
     * @return Returns true if setting up the Ethernet interface was successful
     */
    boolean begin(const MACAddress *address);

    /**
     * Set the size of the network packet buffer in bytes
     *
     * The buffer is used for both sending and receiving packets,
     * so it should be bigger than the biggest packet you want to
     * send or receive.
     *
     * If you don't call this method, the default size is 500 bytes.
     *
     * @note Call this before begin().
     *       It is not possible to change the buffer size once it has been created.
     * @param size The size to make the packet buffer
     */
    void setBufferSize(uint16_t size);

    /**
     * Get the size of the network packet buffer
     *
     * @return The of the packet buffer (in bytes)
     */
    uint16_t getBufferSize();

    /**
     * Manually set the global IPv6 address for the Ethernet Interface
     * from an IPv6Address object
     *
     * This is not needed, if stateless auto-configuration is used.
     *
     * @note Call this before begin().
     * @param address The Global IP address for this Ethernet interface
     */
    void setGlobalAddress(IPv6Address *address);

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
    IPv6Address* getGlobalAddress();

    /**
     * Get the link-local address of the Ethernet Interface
     * This is generated automatically from the MAC address.
     *
     * @return The Link-Local IP address as an IPv6Address object
     */
    IPv6Address* getLinkLocalAddress();

    /**
     * Check to see if an IPv6 address belongs to this Ethernet interface
     *
     * @param address the IPv6Addrss to check
     * @return ture if the
     */
    uint8_t isOurAddress(const IPv6Address *address);

    /**
     * Set the IPv6 address DNS server to use for hostname lookups
     *
     * @param address The DNS Server IP address
     */
    void setDnsServerAddress(IPv6Address *address);

    /**
     * Get the IPv6 address of the DNS server
     *
     * @return The DNS Server address as an IPv6Address object
     */
    IPv6Address* getDnsServerAddress();

    /**
     * Check if there is an IPv6 packet waiting for us and fetch it.
     * If there is no packet available this method returns NULL.
     *
     * @return A pointer to an IPv6Packet, or NULL
     */
    IPv6Packet* receivePacket();

    /**
     * Get a pointer to the packet buffer (the last packet sent or received).
     *
     * @note Unlike receivePacket(), this method will never return NULL.
     * @return A pointer to an IPv6Packet
     */
    IPv6Packet* getPacket();

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
     * @param hostname The hostname to look up
     * @return An IPv6 address or NULL if the lookup failed
     */
    IPv6Address* getHostByName(const char* hostname);


protected:
    IPv6Address linkLocalAddress;  /**< The IPv6 Link-local address of the Ethernet Interface */
    IPv6Address globalAddress;     /**< The IPv6 Global address of the Ethernet Interface */
    IPv6Address dnsServerAddress;  /**< The IPv6 address of the configured DNS server */

    MACAddress routerMac;   /**< The MAC Address of the router to send packets outside of this subnet */

    uint8_t *buffer;        /**< The buffer that sent and received packets are stored in */
    uint16_t bufferSize;    /**< The size of the packet buffer in bytes */


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
    void icmp6SendNS(IPv6Address *targetAddress);

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

#endif
