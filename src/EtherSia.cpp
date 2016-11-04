#include "EtherSia.h"
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
    icmp6SendNS(_linkLocalAddress);

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
    } else if (address.isLinkLocalAllNodes() || address.isSolicitedNodeMulticastAddress(_linkLocalAddress)) {
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

uint16_t EtherSia::receivePacket()
{
    uint16_t len = readFrame(_buffer, sizeof(_buffer));

    if (len) {
        IPv6Packet& packet = (IPv6Packet&)_buffer;
        if (!packet.isValid()) {
            _bufferContainsReceived = false;
            return 0;
        }

        _bufferContainsReceived = true;

        if (packet.protocol() == IP6_PROTO_ICMP6) {
            icmp6ProcessPacket();
        }
    } else {
        // We didn't receive anything
        _bufferContainsReceived = false;
        return 0;
    }

    return len;
}

void EtherSia::prepareSend()
{
    IPv6Packet& packet = (IPv6Packet&)_buffer;

    _bufferContainsReceived = false;

    packet.init();
    if (packet.destination().isLinkLocal()) {
        packet.setSource(_linkLocalAddress);
    } else {
        packet.setSource(_globalAddress);
    }

    packet.setEtherSource(_localMac);

    // FIXME: this might be a link-local MAC
    packet.setEtherDestination(_routerMac);
}

void EtherSia::prepareReply()
{
    IPv6Packet& packet = (IPv6Packet&)_buffer;
    IPv6Address *replySourceAddress;

    _bufferContainsReceived = false;

    if (isOurAddress(packet.destination()) == ADDRESS_TYPE_GLOBAL) {
        replySourceAddress = &_globalAddress;
    } else {
        replySourceAddress = &_linkLocalAddress;
    }

    packet.setHopLimit(IP6_DEFAULT_HOP_LIMIT);

    packet.setDestination(packet.source());
    packet.setSource(*replySourceAddress);

    packet.setEtherDestination(packet.etherSource());
    packet.setEtherSource(_localMac);
}

void EtherSia::send()
{
    IPv6Packet& packet = (IPv6Packet&)_buffer;

    _bufferContainsReceived = false;

    sendFrame(_buffer, packet.length());
}
