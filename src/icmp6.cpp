
#include "EtherSia.h"
#include "ICMPv6Packet.h"


void EtherSia::icmp6NSReply()
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;

    // Does the Neighbour Solicitation target belong to us?
    uint8_t type = isOurAddress(packet.ns.target);
    if (type != ADDRESS_TYPE_LINK_LOCAL && type != ADDRESS_TYPE_GLOBAL) {
        return;
    }

    prepareReply();
    packet.setHopLimit(255);

    // We should now send a neighbor advertisement back to where the neighbor solicication came from.
    packet.setPayloadLength(ICMP6_HEADER_LEN + ICMP6_NA_HEADER_LEN + 8);
    packet.type = ICMP6_TYPE_NA;
    packet.code = 0;
    packet.na.flags = ICMP6_NA_FLAG_S; // Solicited flag.
    memset(packet.na.reserved, 0, sizeof(packet.na.reserved));

    // Set the target link address option
    uint8_t* ptr = _buffer + ICMP6_NA_HEADER_OFFSET + ICMP6_NA_HEADER_LEN;
    ptr[0] = ICMP6_OPTION_TARGET_LINK_ADDRESS;
    ptr[1] = 1;  // Options length, 1 = 8 bytes
    memcpy(&ptr[2], _localMac, 6);

    icmp6PacketSend();
}

void EtherSia::icmp6EchoReply()
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;
    prepareReply();

    packet.type = ICMP6_TYPE_ECHO_REPLY;
    packet.code = 0;

    icmp6PacketSend();
}

void EtherSia::icmp6SendNS(IPv6Address &targetAddress)
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;

    prepareSend();
    packet.setPayloadLength(ICMP6_HEADER_LEN + ICMP6_NS_HEADER_LEN);
    packet.setHopLimit(255);
    packet.source().setZero();
    packet.destination().setSolicitedNodeMulticastAddress(targetAddress);
    packet.etherDestination().setIPv6Multicast(packet.destination());

    packet.type = ICMP6_TYPE_NS;
    packet.code = 0;

    memset(packet.ns.reserved, 0, sizeof(packet.ns.reserved));
    packet.ns.target = targetAddress;

    icmp6PacketSend();
}

void EtherSia::icmp6SendRS()
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;

    prepareSend();
    packet.setPayloadLength(ICMP6_HEADER_LEN + ICMP6_RS_HEADER_LEN);
    packet.setHopLimit(255);
    packet.setSource(_linkLocalAddress);
    packet.destination().setLinkLocalAllRouters();
    packet.etherDestination().setIPv6Multicast(packet.destination());

    packet.type = ICMP6_TYPE_RS;
    packet.code = 0;

    memset(packet.rs.reserved, 0, sizeof(packet.rs.reserved));
    packet.rs.option_type = ICMP6_OPTION_SOURCE_LINK_ADDRESS;
    packet.rs.option_len = 1;
    packet.rs.option_mac = _localMac;

    icmp6PacketSend();
}

void EtherSia::icmp6PacketSend()
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;

    packet.setProtocol(IP6_PROTO_ICMP6);
    packet.checksum = 0;
    packet.checksum = htons(packet.calculateChecksum());

    send();
}

void EtherSia::icmp6ProcessPrefix(struct icmp6_prefix_information *pi)
{
    // Only use prefix if the On-link AND address-configuration flags are set
    // L = Bit 8 = On-link flag
    // A = Bit 7 = Autonomous address-configuration flag
    // 0b11000000 = 0xC0
    if ((pi->flags & 0xC0) != 0xC0) {
        return;
    }

    // We only support a prefix length of 64
    if (pi->prefix_length != 64) {
        return;
    }

    // Only set global address if there isn't one already set
    if (_globalAddress.isZero()) {
        _globalAddress = pi->prefix;
        _globalAddress.setEui64(_localMac);
    }

}

void EtherSia::icmp6ProcessRA()
{
    IPv6Packet& packet = (ICMPv6Packet&)_ptr;
    int16_t remaining = packet.payloadLength() - ICMP6_HEADER_LEN - ICMP6_RA_HEADER_LEN;
    uint8_t *ptr = _buffer + ICMP6_RA_HEADER_OFFSET + ICMP6_RA_HEADER_LEN;

    if (_autoConfigurationEnabled == false) {
        return;
    }

    while(remaining > 0) {
        switch(ptr[0]) {
        case ICMP6_OPTION_SOURCE_LINK_ADDRESS:
            // Store the MAC address of the router
            _routerMac = *((MACAddress*)&ptr[2]);
            break;
        case ICMP6_OPTION_PREFIX_INFORMATION:
            icmp6ProcessPrefix(
                (struct icmp6_prefix_information*)&ptr[2]
            );
            break;
        case ICMP6_OPTION_RECURSIVE_DNS:
            // Recursive DNS Server Option, see RFC6106 for the format
            //    0: Type
            //    1: Length (in units of 8 octets)
            //  2-3: Reserved
            //  4-7: Lifetime (unsigned 32-bit integer in seconds)
            // 8-24: First DNS Server Address
            _dnsServerAddress = *((IPv6Address*)&ptr[8]);
            break;
        }

        remaining -= (8 * ptr[1]);
        ptr += (8 * ptr[1]);
    }
}

MACAddress* EtherSia::icmp6ProcessNA(IPv6Address &expected)
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;
    int16_t remaining = packet.payloadLength() - ICMP6_HEADER_LEN - ICMP6_NA_HEADER_LEN;
    uint8_t *ptr = _buffer + ICMP6_NA_HEADER_OFFSET + ICMP6_NA_HEADER_LEN;

    if (packet.na.target != expected) {
        return NULL;
    }

    // Iterate through options
    while(remaining > 0) {
        switch(ptr[0]) {
        case ICMP6_OPTION_TARGET_LINK_ADDRESS:
            return (MACAddress*)&ptr[2];
        }

        remaining -= (8 * ptr[1]);
        ptr += (8 * ptr[1]);
    }

    return &(packet.etherSource());
}

boolean EtherSia::icmp6ProcessPacket()
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;

    if (isOurAddress(packet.destination()) == 0) {
        // Packet isn't addressed to us
        return false;
    }

    switch(packet.type) {
    case ICMP6_TYPE_NS:
        icmp6NSReply();
        return true;

    case ICMP6_TYPE_ECHO:
        icmp6EchoReply();
        return true;

    case ICMP6_TYPE_RA:
        icmp6ProcessRA();
        return true;

    default:
        // We didn't handle the packet
        return false;
    }
}

boolean EtherSia::icmp6AutoConfigure()
{
    unsigned long nextRouterSolicitation = millis();
    uint8_t count = 0;
    while (_globalAddress.isZero()) {
        if ((long)(millis() - nextRouterSolicitation) >= 0) {
            icmp6SendRS();
            nextRouterSolicitation = millis() + ROUTER_SOLICITATION_TIMEOUT;
            count++;
        }

        receivePacket();

        if (count > ROUTER_SOLICITATION_ATTEMPTS) {
            return false;
        }
    }

    // We have a global IPv6 address - success
    return true;
}

MACAddress* EtherSia::discoverNeighbour(const char* addrstr)
{
    IPv6Address addr(addrstr);
    return discoverNeighbour(addr);
}

MACAddress* EtherSia::discoverNeighbour(IPv6Address& address, uint8_t attempts)
{
    ICMPv6Packet& packet = (ICMPv6Packet&)_ptr;
    unsigned long nextNeighbourSolicitation = millis();
    uint8_t count = 0;

    while (count < attempts) {
        if ((long)(millis() - nextNeighbourSolicitation) >= 0) {
            icmp6SendNS(address);
            nextNeighbourSolicitation = millis() + NEIGHBOUR_SOLICITATION_TIMEOUT;
            count++;
        }

        uint16_t len = receivePacket();
        if (len) {
            if (packet.protocol() == IP6_PROTO_ICMP6 && packet.type == ICMP6_TYPE_NA) {
                MACAddress* neighbourMac = icmp6ProcessNA(address);
                if (neighbourMac) {
                    return neighbourMac;
                }
            }
        }
    }

    return NULL;
}
