
#include "EtherSia.h"


void EtherSia::icmp6_ns_reply()
{
    // Is the Neighbour Solicitation addressed to us?
    if (!is_our_address(&ICMP6_NS_HEADER->target)) {
        return;
    }

    convert_buffer_to_reply();

    // We should now send a neighbor advertisement back to where the neighbor solicication came from.
    IP6_HEADER->length = ntohs(ICMP6_HEADER_LEN + ICMP6_NA_HEADER_LEN);
    ICMP6_HEADER->type = ICMP6_TYPE_NA;
    ICMP6_NA_HEADER->flags = ICMP6_FLAG_S; // Solicited flag.
    ICMP6_NA_HEADER->reserved[0] = ICMP6_NA_HEADER->reserved[1] = ICMP6_NA_HEADER->reserved[2] = 0;
    ICMP6_NA_HEADER->option_type = ICMP6_OPTION_TARGET_LINK_ADDRESS;
    ICMP6_NA_HEADER->option_len = 1;  // Options length, 1 = 8 bytes.
    ICMP6_NA_HEADER->option_mac = enc_mac_addr;

    icmp6_packet_send();
}

void EtherSia::icmp6_echo_reply()
{
    Serial.print(F("Ping from "));
    IP6_HEADER->src.println();

    convert_buffer_to_reply();

    ICMP6_HEADER->type = ICMP6_TYPE_ECHO_REPLY;
    ICMP6_HEADER->code = 0;

    icmp6_packet_send();
}

void EtherSia::icmp6_send_ns(IPv6Address *target_addr)
{
    memset(IP6_HEADER, 0, IP6_HEADER_LEN);
    IP6_HEADER->ver_tc = 0x60;
    IP6_HEADER->length = ntohs(ICMP6_HEADER_LEN + ICMP6_RS_HEADER_LEN);
    IP6_HEADER->proto = IP6_PROTO_ICMP6;
    IP6_HEADER->hop_limit = 255;
    // IP6_HEADER->src = All Zeros
    IP6_HEADER->dest.setSolicitedNodeMulticastAddress(target_addr);

    memset(ETHER_HEADER, 0, ETHER_HEADER_LEN);
    ETHER_HEADER->src = enc_mac_addr;
    ETHER_HEADER->dest.setIPv6Multicast(IP6_HEADER->dest);
    ETHER_HEADER->type = ntohs(ETHER_TYPE_IPV6);

    memset(ICMP6_NS_HEADER, 0, ICMP6_NS_HEADER_LEN);
    ICMP6_NS_HEADER->target = *target_addr;

    icmp6_packet_send();
}

void EtherSia::icmp6_send_rs()
{
    memset(IP6_HEADER, 0, IP6_HEADER_LEN);
    IP6_HEADER->ver_tc = 0x60;
    IP6_HEADER->length = ntohs(ICMP6_HEADER_LEN + ICMP6_RS_HEADER_LEN);
    IP6_HEADER->proto = IP6_PROTO_ICMP6;
    IP6_HEADER->hop_limit = 255;
    IP6_HEADER->src = link_local_addr;
    IP6_HEADER->dest.setLinkLocalAllRouters();

    memset(ETHER_HEADER, 0, ETHER_HEADER_LEN);
    ETHER_HEADER->src = enc_mac_addr;
    ETHER_HEADER->dest.setIPv6Multicast(IP6_HEADER->dest);
    ETHER_HEADER->type = ntohs(ETHER_TYPE_IPV6);

    memset(ICMP6_RS_HEADER, 0, ICMP6_RS_HEADER_LEN);
    ICMP6_HEADER->type = ICMP6_TYPE_RS;
    ICMP6_RS_HEADER->option_type = ICMP6_OPTION_SOURCE_LINK_ADDRESS;
    ICMP6_RS_HEADER->option_len = 1;
    ICMP6_RS_HEADER->option_mac = enc_mac_addr;

    icmp6_packet_send();
}

void EtherSia::icmp6_packet_send()
{
    ICMP6_HEADER->checksum = 0;
    ICMP6_HEADER->checksum = htons(ip6_calculate_checksum());

    send();
}

void EtherSia::icmp6_process_prefix(struct icmp6_prefix_information *pi, MACAddress *router_mac_ptr)
{
    // FIXME: Only if On-link AND stateless address configuration flags set

    // FIXME: Only if prefix-length is 64?

    this->global_addr = pi->prefix;
    this->global_addr.setEui64(&enc_mac_addr);
    Serial.print(F("Global IP="));
    this->global_addr.println();

    this->router_mac = *router_mac_ptr;
    Serial.print(F("Router MAC="));
    this->router_mac.println();
}

void EtherSia::icmp6_process_ra()
{
    int16_t remaining = ntohs(IP6_HEADER->length) - ICMP6_HEADER_LEN - ICMP6_RA_HEADER_LEN;
    uint8_t *ptr = buffer + ICMP6_RA_HEADER_OFFSET + ICMP6_RA_HEADER_LEN;
    uint8_t *pi_ptr = NULL;
    uint8_t *router_mac_ptr = NULL;

    // FIXME: check destination?

    while(remaining > 0) {
        switch(ptr[0]) {
        case ICMP6_OPTION_SOURCE_LINK_ADDRESS:
            router_mac_ptr = &ptr[2];
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

    if (pi_ptr && router_mac_ptr) {
        icmp6_process_prefix(
            (struct icmp6_prefix_information*)pi_ptr,
            (MACAddress *)router_mac_ptr
        );
    }
}

void EtherSia::icmp6_process_packet(uint16_t len)
{
    if (!icmp6_verify_checksum()) {
        Serial.println(F("ICMP6 checksum error."));
        return;
    }

    switch(ICMP6_HEADER->type) {
    case ICMP6_TYPE_NS:
        icmp6_ns_reply();
        break;

    case ICMP6_TYPE_ECHO:
        icmp6_echo_reply();
        break;

    case ICMP6_TYPE_RS:
        // We aren't a router - ignore all Router Solicitations
        break;

    case ICMP6_TYPE_RA:
        icmp6_process_ra();
        break;

    default:
        Serial.print(F("Unknown ICMP6 type="));
        Serial.println(ICMP6_HEADER->type, DEC);
        break;
    }
}

uint8_t EtherSia::icmp6_verify_checksum()
{
    uint16_t packet_checksum = ntohs(ICMP6_HEADER->checksum);

    // Set field in packet to 0 before calculating the checksum
    ICMP6_HEADER->checksum = 0;

    // Does the calculated checksum equal the checksum in the packet?
    return ip6_calculate_checksum() == packet_checksum;
}
