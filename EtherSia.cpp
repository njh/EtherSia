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

    udp_port = 0;
    udp_callback = NULL;

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

bool EtherSia::setDestinationAddress(const char *addr)
{
    return dest_addr.fromString(addr);

//     if (dest_addr.isLinkLocal()) {
//         // FIXME: implement MAC address lookup
//     } else {
//         dest_mac = router_mac;
//     }
}

IPv6Address* EtherSia::getDestinationAddress()
{
    return &dest_addr;
}


// This function is derived from Contiki's uip6.c / upper_layer_chksum()
uint16_t EtherSia::ip6_calculate_checksum()
{
    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    volatile uint16_t newsum = ntohs(IP6_HEADER->length) + IP6_HEADER->proto;

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(IP6_HEADER->src), 32);

    /* Sum the payload header and data */
    uint8_t *payload = (uint8_t *)(this->buffer + IP6_HEADER_OFFSET + IP6_HEADER_LEN);
    newsum = chksum(newsum, payload, ntohs(IP6_HEADER->length));

    return ~newsum;
}

void EtherSia::process_packet(uint16_t len)
{
    if (ETHER_HEADER->type != htons(ETHER_TYPE_IPV6)) {
        return;
    }

#ifdef DEBUG
    if ((IP6_HEADER->ver_tc[0] >> 4) & 0xF != 6) {
        Serial.println("NOT 6");
        return;
    }
#endif

    switch(IP6_HEADER->proto) {
    case IP6_PROTO_ICMP6:
        icmp6_process_packet(len);
        break;

    case IP6_PROTO_TCP:
        Serial.println(F("Got TCP packet"));
        break;

    case IP6_PROTO_UDP:
        udp_process_packet(len);
        break;

    default:
        Serial.print(F("Unknown next header type: "));
        Serial.println(IP6_HEADER->proto, DEC);
        break;
    }
}

void EtherSia::loop()
{
    int len = read(buffer, buffer_len);

    if (len) {
        process_packet(len);
    } else if (global_addr[0] == 0x00) {
        static unsigned long nextRouterSolicitation = millis();
        if ((long)(millis() - nextRouterSolicitation) >= 0) {
            icmp6_send_rs();
            nextRouterSolicitation = millis() + 4000;
        }
    }
}

IPv6Packet* EtherSia::receivePacket()
{
    int len = read(buffer, buffer_len);

    if (len) {
        IPv6Packet *packet = (IPv6Packet*)buffer;

        if (packet->etherType != htons(ETHER_TYPE_IPV6)) {
            return NULL;
        }

// #ifdef DEBUG
//     if ((IP6_HEADER->ver_tc[0] >> 4) & 0xF != 6) {
//         Serial.println("NOT 6");
//         return;
//     }
// #endif

        if (packet->proto == IP6_PROTO_ICMP6) {
            icmp6_process_packet(len);
        }

        return packet;
    } else if (global_addr[0] == 0x00) {
        static unsigned long nextRouterSolicitation = millis();
        if ((long)(millis() - nextRouterSolicitation) >= 0) {
            icmp6_send_rs();
            nextRouterSolicitation = millis() + 4000;
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
