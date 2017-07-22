#include "EtherSia.h"
#include "ICMPv6Packet.h"
#include "util.h"

// https://developers.google.com/speed/public-dns/
static const uint8_t googlePublicDnsAddress[16] PROGMEM = {
    0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88
};

EtherSia::EtherSia()
{
    // Use Google Public DNS by default
    memcpy_P(_dnsServerAddress, googlePublicDnsAddress, sizeof(googlePublicDnsAddress));

    // Use stateless auto-configuration by default
    _autoConfigurationEnabled = true;
}


boolean EtherSia::begin()
{
    boolean success = true;

    // Calculate our link local address
    _linkLocalAddress.setLinkLocalPrefix();
    _linkLocalAddress.setEui64(_localMac);

    // Delay a 'random' amount to stop multiple nodes acting at the same time
    delay(_localMac[5] ^ 0x55);

    // Allow some extra time to let the Ethernet controller to get ready
    delay(500);

    // Send link local Neighbour Solicitation for Duplicate Address Detection
    IPv6Address zero;
    icmp6SendNS(_linkLocalAddress, zero);

    // Perform stateless auto-configuration if enabled
    if (_autoConfigurationEnabled) {
        success = icmp6AutoConfigure();
    }

    // Now there has been a little bit of randomness in network activity
    randomSeed(micros());

    return success;
}

uint8_t EtherSia::isOurAddress(const IPv6Address &address)
{
    if (address == _linkLocalAddress) {
        return ADDRESS_TYPE_LINK_LOCAL;
    } else if (address == _globalAddress) {
        return ADDRESS_TYPE_GLOBAL;
    } else if (address.isLinkLocalAllNodes() ||
               address.isSolicitedNodeMulticastAddress(_linkLocalAddress) ||
               address.isSolicitedNodeMulticastAddress(_globalAddress)) {
        return ADDRESS_TYPE_MULTICAST;
    } else {
        return 0;
    }
}

uint8_t EtherSia::inOurSubnet(const IPv6Address &address)
{
    if (address.isLinkLocal()) {
        return ADDRESS_TYPE_LINK_LOCAL;
    } else if (address.inSameSubnet(_globalAddress)) {
        return ADDRESS_TYPE_GLOBAL;
    } else {
        // Address is in a different subnet
        return 0;
    }
}

boolean EtherSia::setRouter(const char* address) {
    IPv6Address addr(address);
    return setRouter(addr);
}

boolean EtherSia::setRouter(IPv6Address &address) {
    MACAddress *mac = discoverNeighbour(address);
    if (mac == NULL) {
        return false;
    } else {
        _routerMac = *mac;
        return true;
    }
}

boolean EtherSia::checkEthernetAddresses(IPv6Packet &packet) {

    // Check destination address
    if (!packet.etherDestination().isIPv6Multicast() && !(packet.etherDestination() == _localMac)) {
        // Destination not multicast address and not for us
        return false;
    }

    // Check source address
    if (packet.etherSource() == _localMac) {
        // Source address is ours - sometimes we seem to receive packets we sent
        // This has been seen on Ethernet cross-over cables
        return false;
    }

    return true;
}

uint16_t EtherSia::receivePacket()
{
    uint16_t len = readFrame(_buffer, sizeof(_buffer));

    if (len) {
        IPv6Packet& packet = (IPv6Packet&)_ptr;
        if (!packet.isValid() || !checkEthernetAddresses(packet)) {
            _bufferContainsReceived = false;
            return 0;
        }

        _bufferContainsReceived = true;

        if (packet.protocol() == IP6_PROTO_ICMP6) {
            boolean handled = icmp6ProcessPacket();
            if (handled) {
                // Packet has already been handled, don't return it
                return 0;
            }
        }
    } else {
        // We didn't receive anything
        _bufferContainsReceived = false;
        return 0;
    }

    return len;
}

void EtherSia::rejectPacket()
{
    IPv6Packet& packet = (IPv6Packet&)_ptr;

    // Ignore packets we have already replied to
    if (!_bufferContainsReceived)
        return;

    // Ignore multicast packets
    if (packet.destination().isMulticast())
        return;

    if (packet.protocol() == IP6_PROTO_TCP) {
        // Reply with TCP RST packet
        tcpSendRSTReply();
    } else if (packet.protocol() == IP6_PROTO_UDP) {
        // Reply with ICMPv6 Port Unreachable
        icmp6ErrorReply(ICMP6_TYPE_UNREACHABLE, ICMP6_CODE_PORT_UNREACHABLE);
    } else if (packet.protocol() == IP6_PROTO_ICMP6) {
        // Ignore ICMPv6 packets
    } else {
        // Reply with Unrecognised Next Header
        icmp6ErrorReply(ICMP6_TYPE_PARAM_PROB, ICMP6_CODE_UNRECOGNIZED_NH);
    }
}

void EtherSia::prepareSend()
{
    IPv6Packet& packet = (IPv6Packet&)_ptr;

    _bufferContainsReceived = false;

    packet.init();
    if (packet.destination().isLinkLocal() || _globalAddress.isZero()) {
        packet.setSource(_linkLocalAddress);
    } else {
        packet.setSource(_globalAddress);
    }

    packet.setEtherSource(_localMac);
    // caller must set Ethernet Destination
}

void EtherSia::prepareReply()
{
    IPv6Packet& packet = (IPv6Packet&)_ptr;
    IPv6Address destination = packet.source();
    MACAddress etherDestination = packet.etherSource();

    prepareSend();

    packet.setDestination(destination);
    packet.setEtherDestination(etherDestination);
}

void EtherSia::send()
{
    IPv6Packet& packet = (IPv6Packet&)_ptr;

    _bufferContainsReceived = false;

    sendFrame(_buffer, packet.length());
}

void EtherSia::tcpSendRSTReply()
{
    IPv6Packet& packet = (IPv6Packet&)_ptr;
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;
    uint32_t seqNum = htonl(tcpHeader->sequenceNum);
    uint16_t sourcePort = tcpHeader->sourcePort;

    prepareReply();
    tcpHeader->sourcePort = tcpHeader->destinationPort;
    tcpHeader->destinationPort = sourcePort;
    tcpHeader->sequenceNum = 0;
    tcpHeader->acknowledgementNum = htonl(seqNum + 1);
    tcpHeader->dataOffset = (TCP_MINIMUM_HEADER_LEN / 4) << 4;
    tcpHeader->flags = TCP_FLAG_ACK | TCP_FLAG_RST;
    tcpHeader->window = 0;
    tcpHeader->urgentPointer = 0;

    packet.setPayloadLength(TCP_MINIMUM_HEADER_LEN);

    tcpHeader->checksum = 0;
    tcpHeader->checksum = htons(packet.calculateChecksum());

    send();
}
