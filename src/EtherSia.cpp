#include "EtherSia.h"
#include "util.h"

// https://developers.google.com/speed/public-dns/
static const uint8_t googlePublicDnsAddr[16] PROGMEM = {
    0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88
};

EtherSia::EtherSia(int8_t cs) : ENC28J60(cs)
{
    // Use Google Public DNS by default
    memcpy_P(dnsServerAddr, googlePublicDnsAddr, sizeof(googlePublicDnsAddr));

    this->buffer = NULL;
    this->bufferSize = 500;
}


boolean EtherSia::begin(const MACAddress *macaddr)
{
    init(macaddr);

    // Calculate our link local address
    linkLocalAddr.setLinkLocalPrefix();
    linkLocalAddr.setEui64(macaddr);

    // Allocate memory for the packet buffer
    buffer = (uint8_t*)malloc(bufferSize);

    // Delay a 'random' amount to stop multiple nodes acting at the same time
    delay((*macaddr)[5] ^ 0x55);

    // Allow some extra time to let the Ethernet controller to get ready
    delay(500);

    // Send link local Neighbour Solicitation for Duplicate Address Detection
    icmp6SendNS(&linkLocalAddr);

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

void EtherSia::setGlobalAddress(IPv6Address *addr)
{
    globalAddr = *addr;
}

void EtherSia::setGlobalAddress(const char* addr)
{
    globalAddr.fromString(addr);
}

IPv6Address* EtherSia::getGlobalAddress()
{
    return &globalAddr;
}

IPv6Address* EtherSia::getLinkLocalAddress()
{
    return &linkLocalAddr;
}

uint8_t EtherSia::isOurAddress(const IPv6Address *addr)
{
    if (*addr == linkLocalAddr) {
        return ADDRESS_TYPE_LINK_LOCAL;
    } else if (*addr == globalAddr) {
        return ADDRESS_TYPE_GLOBAL;
    } else {
        return 0;
    }
}

void EtherSia::setDnsServerAddress(IPv6Address *addr)
{
    dnsServerAddr = *addr;
}

IPv6Address* EtherSia::getDnsServerAddress()
{
    return &dnsServerAddr;
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

        if (packet->proto == IP6_PROTO_ICMP6) {
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
    if (packet->dest.isLinkLocal()) {
        packet->src = linkLocalAddr;
    } else {
        packet->src = globalAddr;
    }

    packet->etherSrc = enc_mac_addr;

    // FIXME: this might be a link-local MAC
    packet->etherDest = routerMac;
}

void EtherSia::prepareReply()
{
    IPv6Packet *packet = (IPv6Packet*)buffer;
    IPv6Address *replySrcAddr;

    if (isOurAddress(&packet->dest) == ADDRESS_TYPE_GLOBAL) {
        replySrcAddr = &globalAddr;
    } else {
        replySrcAddr = &linkLocalAddr;
    }

    packet->dest = packet->src;
    packet->src = *replySrcAddr;

    packet->etherDest = packet->etherSrc;
    packet->etherSrc = enc_mac_addr;
}

void EtherSia::send()
{
    IPv6Packet *packet = (IPv6Packet*)buffer;
    uint16_t len = ETHER_HEADER_LEN + IP6_HEADER_LEN + ntohs(packet->length);
    ENC28J60::send(buffer, len);
}
