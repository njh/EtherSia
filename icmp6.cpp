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

void EtherSia::process_icmp6(uint16_t len)
{
    Serial.print(F("icmp6 type="));
    Serial.println(ICMP6_HEADER->type, DEC);

    Serial.print(F("icmp6 code="));
    Serial.println(ICMP6_HEADER->code, DEC);

    Serial.print(F("packet checksum=0x"));
    Serial.println(ntohs(ICMP6_HEADER->checksum), HEX);

    Serial.print(F("calculated checksum=0x"));
    Serial.println(icmp6_chksum(), HEX);


    switch(ICMP6_HEADER->type) {
    case ICMP6_TYPE_NS:
        icmp6_ns_reply();
        break;

    case ICMP6_TYPE_ECHO:
        icmp6_echo_reply();
        break;

    default:
        Serial.print(F("Unknown ICMP6 type="));
        Serial.println(ICMP6_HEADER->type, DEC);
        break;
    }
}
