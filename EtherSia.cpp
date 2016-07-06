#include "EtherSia.h"
#include "util.h"


EtherSia::EtherSia(int8_t cs) : ENC28J60(cs)
{
}


boolean EtherSia::begin(const MACAddress *macaddr)
{
    init(macaddr);

    // Calculate our link local address
    link_local_addr.setLinkLocalPrefix();
    link_local_addr.setEui64(macaddr);

    // FIXME: make this configurable
    buffer_len = 800;
    buffer = (uint8_t*)malloc(buffer_len);

    // Delay a 'random' amount to stop multiple nodes acting at the same time
    delay((*macaddr)[5] ^ 0x55);

    // Send link local Neighbour Solicitation for Duplicate Address Detection
    icmp6_send_ns(&link_local_addr);

    return true;
}

uint8_t EtherSia::is_our_address(const IPv6Address *addr)
{
    if (*addr == link_local_addr) {
        return ADDRESS_TYPE_LINK_LOCAL;
    } else if (*addr == global_addr) {
        return ADDRESS_TYPE_GLOBAL;
    } else {
        return 0;
    }
}

IPv6Packet* EtherSia::getPacket()
{
    return (IPv6Packet*)buffer;
}

IPv6Packet* EtherSia::receivePacket()
{
    int len = read(buffer, buffer_len);

    if (len) {
        IPv6Packet *packet = getPacket();
        if (!packet->isValid()) {
            return NULL;
        }

        if (packet->proto == IP6_PROTO_ICMP6) {
            icmp6_process_packet(len);
        }

        return packet;
    } else {
        IPv6Packet *packet = getPacket();
        if (packet) {
            // We didn't receive anything; invalidate the buffer
            packet->etherType = 0;
        }

        if (global_addr.isZero()) {
            static unsigned long nextRouterSolicitation = millis();
            if ((long)(millis() - nextRouterSolicitation) >= 0) {
                icmp6_send_rs();
                nextRouterSolicitation = millis() + 4000;
            }
        }
    }

    return NULL;
}

void EtherSia::prepareSend()
{
    IPv6Packet *packet = (IPv6Packet*)buffer;

    packet->init();
    if (packet->dest.isLinkLocal()) {
        packet->src = link_local_addr;
    } else {
        packet->src = global_addr;
    }

    packet->etherSrc = enc_mac_addr;

    // FIXME: this might be a link-local MAC
    packet->etherDest = router_mac;
}

void EtherSia::prepareReply()
{
    IPv6Packet *packet = (IPv6Packet*)buffer;
    IPv6Address *replySrcAddr;

    if (is_our_address(&packet->dest) == ADDRESS_TYPE_GLOBAL) {
        replySrcAddr = &global_addr;
    } else {
        replySrcAddr = &link_local_addr;
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
