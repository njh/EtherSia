
#include "EtherSia.h"
#include "icmp6.h"


void EtherSia::icmp6NSReply()
{
    IPv6Packet *packet = getPacket();

    // Is the Neighbour Solicitation addressed to us?
    if (!isOurAddress(&ICMP6_NS_HEADER_PTR->target)) {
        return;
    }

    prepareReply();

    // We should now send a neighbor advertisement back to where the neighbor solicication came from.
    packet->length = ntohs(ICMP6_HEADER_LEN + ICMP6_NA_HEADER_LEN);
    ICMP6_HEADER_PTR->type = ICMP6_TYPE_NA;
    ICMP6_NA_HEADER_PTR->flags = ICMP6_NA_FLAG_S; // Solicited flag.
    ICMP6_NA_HEADER_PTR->reserved[0] = ICMP6_NA_HEADER_PTR->reserved[1] = ICMP6_NA_HEADER_PTR->reserved[2] = 0;
    ICMP6_NA_HEADER_PTR->option_type = ICMP6_OPTION_TARGET_LINK_ADDRESS;
    ICMP6_NA_HEADER_PTR->option_len = 1;  // Options length, 1 = 8 bytes.
    ICMP6_NA_HEADER_PTR->option_mac = enc_mac_addr;

    icmp6PacketSend();
}

void EtherSia::icmp6EchoReply()
{
    IPv6Packet *packet = getPacket();

    Serial.print(F("Ping from "));
    packet->src.println();

    prepareReply();

    ICMP6_HEADER_PTR->type = ICMP6_TYPE_ECHO_REPLY;
    ICMP6_HEADER_PTR->code = 0;

    icmp6PacketSend();
}

void EtherSia::icmp6SendNS(IPv6Address *target_addr)
{
    IPv6Packet *packet = getPacket();

    packet->init();
    packet->length = ntohs(ICMP6_HEADER_LEN + ICMP6_NS_HEADER_LEN);
    packet->proto = IP6_PROTO_ICMP6;
    packet->hopLimit = 255;
    packet->src.setZero();
    packet->dest.setSolicitedNodeMulticastAddress(target_addr);

    packet->etherSrc = enc_mac_addr;
    packet->etherDest.setIPv6Multicast(packet->dest);

    ICMP6_HEADER_PTR->type = ICMP6_TYPE_NS;
    ICMP6_HEADER_PTR->code = 0;

    memset(ICMP6_NS_HEADER_PTR, 0, ICMP6_NS_HEADER_LEN);
    ICMP6_NS_HEADER_PTR->target = *target_addr;

    icmp6PacketSend();
}

void EtherSia::icmp6SendRS()
{
    IPv6Packet *packet = getPacket();

    packet->init();
    packet->length = ntohs(ICMP6_HEADER_LEN + ICMP6_RS_HEADER_LEN);
    packet->proto = IP6_PROTO_ICMP6;
    packet->hopLimit = 255;
    packet->src = linkLocalAddr;
    packet->dest.setLinkLocalAllRouters();

    packet->etherSrc = enc_mac_addr;
    packet->etherDest.setIPv6Multicast(packet->dest);

    ICMP6_HEADER_PTR->type = ICMP6_TYPE_RS;
    ICMP6_HEADER_PTR->code = 0;

    memset(ICMP6_RS_HEADER_PTR, 0, ICMP6_RS_HEADER_LEN);
    ICMP6_RS_HEADER_PTR->option_type = ICMP6_OPTION_SOURCE_LINK_ADDRESS;
    ICMP6_RS_HEADER_PTR->option_len = 1;
    ICMP6_RS_HEADER_PTR->option_mac = enc_mac_addr;

    icmp6PacketSend();
}

void EtherSia::icmp6PacketSend()
{
    IPv6Packet *packet = getPacket();

    ICMP6_HEADER_PTR->checksum = 0;
    ICMP6_HEADER_PTR->checksum = htons(packet->calculateChecksum());

    send();
}

void EtherSia::icmp6ProcessPrefix(struct icmp6_prefix_information *pi, MACAddress *routerMacPtr)
{
    // Only use prefix if the On-link AND address-configuration flags are set
    // L = Bit 8 = On-link flag
    // A = Bit 7 = Autonomous address-configuration flag
    // 0b11000000 = 0xC0
    if (pi->flags & 0xC0 != 0xC0) {
        return;
    }

    // We only support a prefix length of 64
    if (pi->prefix_length != 64) {
        return;
    }

    // Only set global address if there isn't one already set
    if (globalAddr.isZero()) {
        globalAddr = pi->prefix;
        globalAddr.setEui64(&enc_mac_addr);
    }

    // Store the MAC address of the router
    routerMac = *routerMacPtr;
}

void EtherSia::icmp6ProcessRA()
{
    IPv6Packet *packet = getPacket();
    int16_t remaining = ntohs(packet->length) - ICMP6_HEADER_LEN - ICMP6_RA_HEADER_LEN;
    uint8_t *ptr = (uint8_t*)packet + ICMP6_RA_HEADER_OFFSET + ICMP6_RA_HEADER_LEN;
    uint8_t *pi_ptr = NULL;
    uint8_t *routerMac_ptr = NULL;

    // FIXME: check destination?

    while(remaining > 0) {
        switch(ptr[0]) {
        case ICMP6_OPTION_SOURCE_LINK_ADDRESS:
            routerMac_ptr = &ptr[2];
            break;
        case ICMP6_OPTION_PREFIX_INFORMATION:
            pi_ptr = &ptr[2];
            break;
        case ICMP6_OPTION_MTU:
            // FIXME: do something with the MTU?
            break;
        }

        remaining -= (8 * ptr[1]);
        ptr += (8 * ptr[1]);
    }

    if (pi_ptr && routerMac_ptr) {
        icmp6ProcessPrefix(
            (struct icmp6_prefix_information*)pi_ptr,
            (MACAddress *)routerMac_ptr
        );
    }
}

void EtherSia::icmp6ProcessPacket(uint16_t len)
{
    if (!icmp6VerifyChecksum()) {
        Serial.println(F("ICMP6 checksum error."));
        return;
    }

    switch(ICMP6_HEADER_PTR->type) {
    case ICMP6_TYPE_NS:
        icmp6NSReply();
        break;

    case ICMP6_TYPE_ECHO:
        icmp6EchoReply();
        break;

    case ICMP6_TYPE_RS:
        // We aren't a router - ignore all Router Solicitations
        break;

    case ICMP6_TYPE_RA:
        icmp6ProcessRA();
        break;

    default:
        Serial.print(F("Unknown ICMP6 type="));
        Serial.println(ICMP6_HEADER_PTR->type, DEC);
        break;
    }
}

uint8_t EtherSia::icmp6VerifyChecksum()
{
    IPv6Packet *packet = getPacket();
    uint16_t packetChecksum = ntohs(ICMP6_HEADER_PTR->checksum);

    // Set field in packet to 0 before calculating the checksum
    ICMP6_HEADER_PTR->checksum = 0;

    // Does the calculated checksum equal the checksum in the packet?
    return packet->calculateChecksum() == packetChecksum;
}
