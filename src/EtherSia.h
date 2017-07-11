/**
 * Main header file for EtherSia - include this in your project
 * @file EtherSia.h
 */

#ifndef EtherSia_H
#define EtherSia_H

#include <Arduino.h>
#include <stdint.h>

#include "esendian.h"

#include "MACAddress.h"
#include "IPv6Address.h"
#include "IPv6Packet.h"
#include "Socket.h"
#include "UDPSocket.h"

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
#define ETHERSIA_MAX_PACKET_SIZE       600



/** How often to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_TIMEOUT      (3000)

/** How many times to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_ATTEMPTS     (4)

/** How often to send Neighbour Solicitation (NS) packets */
#define NEIGHBOUR_SOLICITATION_TIMEOUT   (500)

/** How many times to send Neighbour Solicitation (NS) packets */
#define NEIGHBOUR_SOLICITATION_ATTEMPTS  (5)


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
     * Configure the Ethernet interface and get things ready
     *
     * If auto-configuration has not been disabled, then
     * stateless auto-configuration will start - attempting to
     * get an IP address and Router address using IGMPv6.
     *
     * @return Returns true if setting up the Ethernet interface was successful
     */
    /*virtual*/ boolean begin();
    // FIXME: virtual is disabled because it seems to trigger Arduino/issues/3972

    /**
     * Disable stateless auto-configuration (SLAAC)
     *
     * Disables sending Router Solicitations and accepting Router Advertisements.
     *
     * Disable auto-configuration if you would like to use a statically
     * configured global address, or only want to use link-local addressing.
     *
     */
    inline void disableAutoconfiguration() {
        _autoConfigurationEnabled = false;
    }

    /**
     * Enable stateless auto-configuration (SLAAC)
     *
     * @note Stateless auto-configuration is enabled by default.
     */
    inline void enableAutoconfiguration() {
        _autoConfigurationEnabled = true;
    }

    /**
     * Manually set the global IPv6 address for the Ethernet Interface
     * from an IPv6Address object
     *
     * If using a static global address, it would be a good idea to disable
     * auto-configuration using disableAutoconfiguration() before calling begin().
     *
     * @param address The Global IP address for this Ethernet interface
     */
    inline void setGlobalAddress(IPv6Address &address) {
        _globalAddress = address;
    }

    /**
     * Manually set the global IPv6 address for the Ethernet Interface
     * from human readable string.
     *
     * If using a static global address, it would be a good idea to disable
     * auto-configuration using disableAutoconfiguration() before calling begin().
     *
     * @param address The Global IP address for this Ethernet interface
     */
    inline void setGlobalAddress(const char* address) {
        _globalAddress.fromString(address);
    }

    /**
     * Get the global IPv6 address of the Ethernet Interface
     *
     * If this is called after begin(), then it will return
     * the IP address assigned during stateless auto-configuration.
     *
     * @return The Global IP address as an IPv6Address object
     */
    inline IPv6Address& globalAddress() {
        return _globalAddress;
    }

    /**
     * Get the link-local address of the Ethernet Interface
     * This is generated automatically from the MAC address.
     *
     * @return The Link-Local IP address as an IPv6Address object
     */
    inline IPv6Address& linkLocalAddress() {
        return _linkLocalAddress;
    }

    /**
     * Get the MAC address of the router on the local subnet
     *
     * This is used as the Ethernet destination address for
     * packets that need to be sent outside of the subnet.
     *
     * Typically this is set during the stateless auto-configuration process
     * when you call begin().
     *
     * @return The MAC address of the router
     */
    inline MACAddress& routerMac() {
        return _routerMac;
    }

    /**
     * Manually set the MAC address of the router on the local subnet
     *
     * This is used as the Ethernet destination address for
     * packets that need to be sent outside of the subnet.
     *
     * @note You don't normally need to use this if auto-configuration is enabled, which it is by default.
     * @param routerMac The MAC address of the router
     */
    inline void setRouter(MACAddress& routerMac) {
        _routerMac = routerMac;
    }

    /**
     * Manually set the IPv6 address of the router on the local subnet
     *
     * Neighbour discovery is used to resolve this to a MAC address.
     *
     * @note You don't normally need to use this if auto-configuration is enabled, which it is by default.
     * @param address The IPv6 address of the router, as C string
     * @return true if setting the router address was successful
     */
    boolean setRouter(const char* address);

    /**
     * Manually set the IPv6 address of the router on the local subnet
     *
     * Neighbour discovery is used to resolve this to a MAC address.
     *
     * @note You don't normally need to use this if auto-configuration is enabled, which it is by default.
     * @param address The IPv6 address of the router
     * @return true if setting the router address was successful
     */
    boolean setRouter(IPv6Address &address);

    /**
     * Check to see if an IPv6 address belongs to this Ethernet interface
     *
     * @param address the IPv6Addrss to check
     * @return the type of address IPv6AddressType or 0 if it is not our address
     */
    uint8_t isOurAddress(const IPv6Address &address);

    /**
     * Check if an address is in the same subnet as us
     *
     * @param address the IPv6Addrss to check
     * @return the type of address IPv6AddressType or 0 if it is not in the same subnet
     * @note assumes that the subnet mask is /64
     */
    uint8_t inOurSubnet(const IPv6Address &address);

    /**
     * Set the IPv6 address DNS server to use for hostname lookups
     *
     * @param address The DNS Server IP address
     */
    inline void setDnsServerAddress(IPv6Address &address) {
        _dnsServerAddress = address;
    }

    /**
     * Get the IPv6 address of the DNS server
     *
     * @return The DNS Server address as an IPv6Address object
     */
    inline IPv6Address& dnsServerAddress() {
        return _dnsServerAddress;
    }

    /**
     * Check if there is an IPv6 packet waiting for us and copy it into the buffer.
     * If there is no packet available this method returns 0.
     *
     * @return The length of the packet, or 0 if no packet was received
     */
    uint16_t receivePacket();

    /**
     * Check the received packet, and reply with a rejection packet.
     *
     * Calling this function is optional. If you don't call it the
     * packet will just be ignored. This is similar to the difference
     * between drop and reject on a firewall.
     *
     * - If the packet has already been replied to, it is ignored
     * - If it was sent to a multicast address, it is ignored
     * - If it is a TCP packet, a TCP RST reply is sent
     * - It it was sent to a UDP port, an ICMPv6 Destination Unreachable reply is sent
     * - If it is an unknown protocol, an ICMPv6 Unrecognised Next Header reply is sent
     *
     */
    void rejectPacket();

    /**
     * Check if the packet buffer contains a valid received packet
     *
     * @return
     * * true: the buffer contains a valid packet received over the network
     * * false: the buffer contains a packet we generated, or is invalid in some way
     */
    inline boolean bufferContainsReceived() {
        return _bufferContainsReceived;
    }

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
    inline IPv6Packet& packet()
    {
        return (IPv6Packet&)_ptr;
    }

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
     * Perform Neighbour Discovery for an IPv6 address on the local subnet
     *
     * This method takes an IPv6 address and resolves it to a MAC address.
     *
     * It is recommended that this method is called within setup(),
     * to avoid packets being lost within loop().
     *
     * @note You probably don't need to call this function directly.
     * @param address The IPv6 address to perform discovery on
     * @return An pointer to a MAC address or NULL if the discovery failed
     */
    MACAddress* discoverNeighbour(const char* address);

    /**
     * Perform Neighbour Discovery for an IPv6 address on the local subnet
     *
     * This method takes an IPv6 address and resolves it to a MAC address.
     *
     * It is recommended that this method is called within setup(),
     * to avoid packets being lost within loop().
     *
     * @note You probably don't need to call this function directly.
     * @param address The IPv6 address to perform discovery on
     * @param attempts The number of ICMPv6 packets to send before giving up
     * @return An pointer to a MAC address or NULL if the discovery failed
     */
    MACAddress* discoverNeighbour(IPv6Address& address, uint8_t attempts=NEIGHBOUR_SOLICITATION_ATTEMPTS);

    /**
     * Send a reply with a TCP RST packet
     */
    void tcpSendRSTReply();

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
    union {
        uint8_t _buffer[ETHERSIA_MAX_PACKET_SIZE];
        void* _ptr;
    };

    /** Flag indicating if the buffer contains a valid packet we received */
    boolean _bufferContainsReceived;

    /** Flag indicating if the buffer contains a valid packet we received */
    boolean _autoConfigurationEnabled;

    /**
     * Checks the Ethernet Layer 2 addresses
     * @return true if packet should be accepted
     */
    boolean checkEthernetAddresses(IPv6Packet &packet);

    /**
     * Process a received ICMPv6 packet in the packet buffer
     *
     * Responds to ICMPv6 Pings, Neighbour Solicitations and Router Solicitations
     */
    boolean icmp6ProcessPacket();

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
     * @param sourceAddress The IPv6 address to send from
     */
    void icmp6SendNS(IPv6Address &targetAddress, IPv6Address &sourceAddress);

    /**
     * Send a ICMPv6 Neighbour Solicitation (NS) for specified IPv6 Address
     *
     * @param type The IPv6 address to be resolved
     * @param code The IPv6 address to send from
     */
    void icmp6ErrorReply(uint8_t type, uint8_t code);

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
     * Handle a ICMPv6 Neighbour Advertisement (NA) packet
     *
     * @param expected The IPv6 address we are expecting a response about
     * @return Target Link Address, if successful, otherwise NULL
     */
    MACAddress* icmp6ProcessNA(IPv6Address &expected);

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

#include "PingClient.h"
#include "TCPServer.h"
#include "HTTPServer.h"
#include "TFTPServer.h"
#include "Syslog.h"

#include "enc28j60.h"
#include "w5100.h"
#include "w5500.h"

#ifndef ARDUINO
#include "dummy.h"
#include "LinuxSocket.h"
#endif


#endif
