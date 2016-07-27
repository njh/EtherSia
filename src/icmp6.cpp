
#include "EtherSia.h"
#include "icmp6.h"


void EtherSia::icmp6NSReply()
{
    IPv6Packet *packet = getPacket();

    // Does the Neighbour Solicitation target belong to us?
    uint8_t type = isOurAddress(ICMP6_NS_HEADER_PTR->target);
    if (type != ADDRESS_TYPE_LINK_LOCAL && type != ADDRESS_TYPE_GLOBAL) {
        return;
    }

    prepareReply();

    // We should now send a neighbor advertisement back to where the neighbor solicication came from.
    packet->length = ntohs(ICMP6_HEADER_LEN + ICMP6_NA_HEADER_LEN);
    ICMP6_HEADER_PTR->type = ICMP6_TYPE_NA;
    ICMP6_NA_HEADER_PTR->flags = ICMP6_NA_FLAG_S; // Solicited flag.
    memset(ICMP6_NA_HEADER_PTR->reserved, 0, sizeof(ICMP6_NA_HEADER_PTR->reserved));
    ICMP6_NA_HEADER_PTR->option_type = ICMP6_OPTION_TARGET_LINK_ADDRESS;
    ICMP6_NA_HEADER_PTR->option_len = 1;  // Options length, 1 = 8 bytes.
    ICMP6_NA_HEADER_PTR->option_mac = encMacAddress;

    icmp6PacketSend();
}

void EtherSia::icmp6EchoReply()
{
    prepareReply();

    ICMP6_HEADER_PTR->type = ICMP6_TYPE_ECHO_REPLY;
    ICMP6_HEADER_PTR->code = 0;

    icmp6PacketSend();
}

void EtherSia::icmp6SendNS(IPv6Address &targetAddress)
{
    IPv6Packet *packet = getPacket();

    packet->init();
    packet->length = ntohs(ICMP6_HEADER_LEN + ICMP6_NS_HEADER_LEN);
    packet->hopLimit = 255;
    packet->source.setZero();
    packet->destination.setSolicitedNodeMulticastAddress(targetAddress);
    packet->etherDestination.setIPv6Multicast(packet->destination);

    ICMP6_HEADER_PTR->type = ICMP6_TYPE_NS;
    ICMP6_HEADER_PTR->code = 0;

    memset(ICMP6_NS_HEADER_PTR->reserved, 0, sizeof(ICMP6_NS_HEADER_PTR->reserved));
    ICMP6_NS_HEADER_PTR->target = targetAddress;

    icmp6PacketSend();
}

void EtherSia::icmp6SendRS()
{
    IPv6Packet *packet = getPacket();

    packet->init();
    packet->length = ntohs(ICMP6_HEADER_LEN + ICMP6_RS_HEADER_LEN);
    packet->hopLimit = 255;
    packet->source = linkLocalAddress;
    packet->destination.setLinkLocalAllRouters();
    packet->etherDestination.setIPv6Multicast(packet->destination);

    ICMP6_HEADER_PTR->type = ICMP6_TYPE_RS;
    ICMP6_HEADER_PTR->code = 0;

    memset(ICMP6_RS_HEADER_PTR->reserved, 0, sizeof(ICMP6_RS_HEADER_PTR->reserved));
    ICMP6_RS_HEADER_PTR->option_type = ICMP6_OPTION_SOURCE_LINK_ADDRESS;
    ICMP6_RS_HEADER_PTR->option_len = 1;
    ICMP6_RS_HEADER_PTR->option_mac = encMacAddress;

    icmp6PacketSend();
}

void EtherSia::icmp6PacketSend()
{
    IPv6Packet *packet = getPacket();

    packet->protocol = IP6_PROTO_ICMP6;
    ICMP6_HEADER_PTR->checksum = 0;
    ICMP6_HEADER_PTR->checksum = htons(packet->calculateChecksum());

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
    if (globalAddress.isZero()) {
        globalAddress = pi->prefix;
        globalAddress.setEui64(encMacAddress);
    }

}

void EtherSia::icmp6ProcessRA()
{
    IPv6Packet *packet = getPacket();
    int16_t remaining = ntohs(packet->length) - ICMP6_HEADER_LEN - ICMP6_RA_HEADER_LEN;
    uint8_t *ptr = (uint8_t*)packet + ICMP6_RA_HEADER_OFFSET + ICMP6_RA_HEADER_LEN;

    while(remaining > 0) {
        switch(ptr[0]) {
        case ICMP6_OPTION_SOURCE_LINK_ADDRESS:
            // Store the MAC address of the router
            routerMac = *((MACAddress*)&ptr[2]);
            break;
        case ICMP6_OPTION_PREFIX_INFORMATION:
            icmp6ProcessPrefix(
                (struct icmp6_prefix_information*)&ptr[2]
            );
            break;
        }

        remaining -= (8 * ptr[1]);
        ptr += (8 * ptr[1]);
    }
}

void EtherSia::icmp6ProcessPacket()
{
    IPv6Packet *packet = getPacket();

    if (isOurAddress(packet->destination) == 0) {
        // Packet isn't addressed to us
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
    }
}

boolean EtherSia::icmp6AutoConfigure()
{
    unsigned long nextRouterSolicitation = millis();
    uint8_t count = 0;
    while (globalAddress.isZero()) {
        receivePacket();

        if ((long)(millis() - nextRouterSolicitation) >= 0) {
            icmp6SendRS();
            nextRouterSolicitation = millis() + ROUTER_SOLICITATION_TIMEOUT;
            count++;
        }

        if (count > ROUTER_SOLICITATION_ATTEMPTS) {
            return false;
        }
    }

    // We have a global IPv6 address - success
    return true;
}
