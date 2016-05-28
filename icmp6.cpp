#include <stdint.h>

#include "EtherSia.h"
#include "packet_headers.h"


static uint16_t
chksum(uint16_t sum, const uint8_t *data, uint16_t len)
{
    uint16_t t;
    const uint8_t *dataptr;
    const uint8_t *last_byte;

    dataptr = data;
    last_byte = data + len - 1;

    while(dataptr < last_byte) {   /* At least two more bytes */
        t = (dataptr[0] << 8) + dataptr[1];
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
        dataptr += 2;
    }

    if(dataptr == last_byte) {
        t = (dataptr[0] << 8) + 0;
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
    }

    /* Return sum in host byte order. */
    return sum;
}

uint16_t
EtherSia::icmp6_chksum()
{
    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    volatile uint16_t newsum = ntohs(IP6_HEADER->length) + IP6_HEADER->proto;

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(IP6_HEADER->src), 32);

    /* Sum ICMP6 header and data. */
    ICMP6_HEADER->checksum = 0;
    newsum = chksum(newsum, (uint8_t *)ICMP6_HEADER, ntohs(IP6_HEADER->length));

    return ~newsum;
}

void EtherSia::icmp6_ns_reply()
{
    // Is the Neighbour Solicitation addressed to us?
    if (!is_our_address(ICMP6_NA_HEADER->target)) {
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
    memcpy(ICMP6_NA_HEADER->option_mac, enc_mac_addr, sizeof(enc_mac_addr));
    ICMP6_HEADER->checksum = 0;
    ICMP6_HEADER->checksum = htons(icmp6_chksum());

    ip6_packet_send();
}

void EtherSia::icmp6_echo_reply()
{
    convert_buffer_to_reply();

    ICMP6_HEADER->type = ICMP6_TYPE_ECHO_REPLY;
    ICMP6_HEADER->code = 0;
    ICMP6_HEADER->checksum = 0;
    ICMP6_HEADER->checksum = htons(icmp6_chksum());

    ip6_packet_send();
}

void EtherSia::icmp6_send_rs()
{
    memset(ETHER_HEADER, 0, ETHER_HEADER_LEN);
    memcpy(ETHER_HEADER->src, enc_mac_addr, 6);
    ETHER_HEADER->dest[0] = 0x33;
    ETHER_HEADER->dest[1] = 0x33;
    ETHER_HEADER->dest[5] = 0x02;
    ETHER_HEADER->type = ntohs(ETHER_TYPE_IPV6);

    memset(IP6_HEADER, 0, IP6_HEADER_LEN);
    IP6_HEADER->ver_tc = 0x60;
    IP6_HEADER->length = ntohs(ICMP6_HEADER_LEN + ICMP6_RS_HEADER_LEN);
    IP6_HEADER->proto = IP6_PROTO_ICMP6;
    IP6_HEADER->hop_limit = 255;
    memcpy(IP6_HEADER->src, link_local_addr, 16);
    IP6_HEADER->dest[0] = 0xFF;
    IP6_HEADER->dest[1] = 0x02;
    IP6_HEADER->dest[15] = 0x02;

    memset(ICMP6_RS_HEADER, 0, ICMP6_RS_HEADER_LEN);
    ICMP6_HEADER->type = ICMP6_TYPE_RS;
    ICMP6_RS_HEADER->option_type = ICMP6_OPTION_SOURCE_LINK_ADDRESS;
    ICMP6_RS_HEADER->option_len = 1;
    memcpy(ICMP6_RS_HEADER->option_mac, enc_mac_addr, sizeof(enc_mac_addr));

    ICMP6_HEADER->checksum = 0;
    ICMP6_HEADER->checksum = htons(icmp6_chksum());

    ip6_packet_send();
}

void EtherSia::icmp6_process_prefix(struct icmp6_prefix_information *pi, uint8_t *router_mac_ptr)
{
    // FIXME: Only if On-link AND stateless address configuration flags set

    // FIXME: Only if prefix-length is 64?

    memcpy(this->global_addr, pi->prefix, 16);
    set_eui_64(this->global_addr, enc_mac_addr);
    Serial.print(F("Global IP="));
    print_address(this->global_addr);

    memcpy(this->router_mac, router_mac_ptr, 6);
    Serial.print(F("Router MAC="));
    print_mac(this->router_mac);
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
            router_mac_ptr
        );
    }
}

void EtherSia::icmp6_process_packet(uint16_t len)
{

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
