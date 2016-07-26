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
    memcpy_P(dnsServerAddress, googlePublicDnsAddress, sizeof(googlePublicDnsAddress));

    this->buffer = NULL;
    this->bufferSize = 500;
}


boolean EtherSia::begin(const MACAddress *macAddress)
{
    init(macAddress);

    // Calculate our link local address
    linkLocalAddress.setLinkLocalPrefix();
    linkLocalAddress.setEui64(macAddress);

    // Allocate memory for the packet buffer
    buffer = (uint8_t*)malloc(bufferSize);

    // Delay a 'random' amount to stop multiple nodes acting at the same time
    delay((*macAddress)[5] ^ 0x55);

    // Allow some extra time to let the Ethernet controller to get ready
    delay(500);

    // Send link local Neighbour Solicitation for Duplicate Address Detection
    icmp6SendNS(&linkLocalAddress);

    // Perform stateless auto-configuration
    boolean result = icmp6AutoConfigure();

    // Now there has been a little bit of randomness in network activity
    randomSeed(micros());

    return result;
}

void EtherSia::setBufferSize(uint16_t size)
{
    this->bufferSize = size;
}

uint16_t EtherSia::getBufferSize()
{
    return this->bufferSize;
}

void EtherSia::setGlobalAddress(IPv6Address *address)
{
    globalAddress = *address;
}

void EtherSia::setGlobalAddress(const char* address)
{
    globalAddress.fromString(address);
}

IPv6Address* EtherSia::getGlobalAddress()
{
    return &globalAddress;
}

IPv6Address* EtherSia::getLinkLocalAddress()
{
    return &linkLocalAddress;
}

uint8_t EtherSia::isOurAddress(const IPv6Address *address)
{
    if (*address == linkLocalAddress) {
        return ADDRESS_TYPE_LINK_LOCAL;
    } else if (*address == globalAddress) {
        return ADDRESS_TYPE_GLOBAL;
    } else if (address->isLinkLocalAllNodes() || address->isSolicitedNodeMulticastAddress(&linkLocalAddress)) {
        return ADDRESS_TYPE_MULTICAST;
    } else {
        return 0;
    }
}

void EtherSia::setDnsServerAddress(IPv6Address *address)
{
    dnsServerAddress = *address;
}

IPv6Address* EtherSia::getDnsServerAddress()
{
    return &dnsServerAddress;
}

IPv6Packet* EtherSia::getPacket()
{
    return (IPv6Packet*)buffer;
}

IPv6Packet* EtherSia::receivePacket()
{
    int len = read(buffer, bufferSize);

    if (len) {
        IPv6Packet *packet = getPacket();
        if (!packet->isValid()) {
            return NULL;
        }

        if (packet->protocol == IP6_PROTO_ICMP6) {
            icmp6ProcessPacket();
        }

        return packet;
    } else {
        IPv6Packet *packet = getPacket();

        // We didn't receive anything; invalidate the buffer
        packet->etherType = 0;
    }

    return NULL;
}

void EtherSia::prepareSend()
{
    IPv6Packet *packet = (IPv6Packet*)buffer;

    packet->init();
    if (packet->destination.isLinkLocal()) {
        packet->source = linkLocalAddress;
    } else {
        packet->source = globalAddress;
    }

    packet->etherSource = encMacAddress;

    // FIXME: this might be a link-local MAC
    packet->etherDestination = routerMac;
}

void EtherSia::prepareReply()
{
    IPv6Packet *packet = (IPv6Packet*)buffer;
    IPv6Address *replySourceAddress;

    if (isOurAddress(&packet->destination) == ADDRESS_TYPE_GLOBAL) {
        replySourceAddress = &globalAddress;
    } else {
        replySourceAddress = &linkLocalAddress;
    }

    packet->destination = packet->source;
    packet->source = *replySourceAddress;

    packet->etherDestination = packet->etherSource;
    packet->etherSource = encMacAddress;
}

void EtherSia::send()
{
    IPv6Packet *packet = (IPv6Packet*)buffer;
    uint16_t len = ETHER_HEADER_LEN + IP6_HEADER_LEN + ntohs(packet->length);
    ENC28J60::send(buffer, len);
}
