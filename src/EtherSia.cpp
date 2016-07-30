#include "EtherSia.h"
#include "util.h"

// https://developers.google.com/speed/public-dns/
static const uint8_t googlePublicDnsAddress[16] PROGMEM = {
    0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88
};

EtherSia::EtherSia(int8_t cs) : ENC28J60(cs)
{
    // Use Google Public DNS by default
    memcpy_P(_dnsServerAddress, googlePublicDnsAddress, sizeof(googlePublicDnsAddress));
}


boolean EtherSia::begin(const MACAddress &macAddress)
{
    init(macAddress);

    // Calculate our link local address
    _linkLocalAddress.setLinkLocalPrefix();
    _linkLocalAddress.setEui64(macAddress);

    // Delay a 'random' amount to stop multiple nodes acting at the same time
    delay(macAddress[5] ^ 0x55);

    // Allow some extra time to let the Ethernet controller to get ready
    delay(500);

    // Send link local Neighbour Solicitation for Duplicate Address Detection
    icmp6SendNS(_linkLocalAddress);

    // Perform stateless auto-configuration
    boolean result = icmp6AutoConfigure();

    // Now there has been a little bit of randomness in network activity
    randomSeed(micros());

    return result;
}

void EtherSia::setGlobalAddress(IPv6Address &address)
{
    _globalAddress = address;
}

void EtherSia::setGlobalAddress(const char* address)
{
    _globalAddress.fromString(address);
}

IPv6Address& EtherSia::globalAddress()
{
    return _globalAddress;
}

IPv6Address& EtherSia::linkLocalAddress()
{
    return _linkLocalAddress;
}

uint8_t EtherSia::isOurAddress(const IPv6Address &address)
{
    if (address == _linkLocalAddress) {
        return ADDRESS_TYPE_LINK_LOCAL;
    } else if (address == _globalAddress) {
        return ADDRESS_TYPE_GLOBAL;
    } else if (address.isLinkLocalAllNodes() || address.isSolicitedNodeMulticastAddress(_linkLocalAddress)) {
        return ADDRESS_TYPE_MULTICAST;
    } else {
        return 0;
    }
}

void EtherSia::setDnsServerAddress(IPv6Address &address)
{
    _dnsServerAddress = address;
}

IPv6Address& EtherSia::dnsServerAddress()
{
    return _dnsServerAddress;
}

IPv6Packet& EtherSia::packet()
{
    return (IPv6Packet&)_buffer;
}

uint16_t EtherSia::receivePacket()
{
    IPv6Packet& packet = (IPv6Packet&)_buffer;
    uint16_t len = read(_buffer, sizeof(_buffer));

    if (len) {
        if (!packet.isValid()) {
            return 0;
        }

        if (packet.protocol() == IP6_PROTO_ICMP6) {
            icmp6ProcessPacket();
        }
    } else {
        // We didn't receive anything; invalidate the buffer
        packet.invalidate();
        return 0;
    }

    return len;
}

void EtherSia::prepareSend()
{
    IPv6Packet& packet = (IPv6Packet&)_buffer;

    packet.init();
    if (packet.destination().isLinkLocal()) {
        packet.setSource(_linkLocalAddress);
    } else {
        packet.setSource(_globalAddress);
    }

    packet.setEtherSource(_encMacAddress);

    // FIXME: this might be a link-local MAC
    packet.setEtherDestination(_routerMac);
}

void EtherSia::prepareReply()
{
    IPv6Packet& packet = (IPv6Packet&)_buffer;
    IPv6Address *replySourceAddress;

    if (isOurAddress(packet.destination()) == ADDRESS_TYPE_GLOBAL) {
        replySourceAddress = &_globalAddress;
    } else {
        replySourceAddress = &_linkLocalAddress;
    }

    packet.setDestination(packet.source());
    packet.setSource(*replySourceAddress);

    packet.setEtherDestination(packet.etherSource());
    packet.setEtherSource(_encMacAddress);
}

void EtherSia::send()
{
    IPv6Packet& packet = (IPv6Packet&)_buffer;
    ENC28J60::send(_buffer, packet.length());
}
