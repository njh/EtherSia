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
    struct ip6_header *ip6 = (struct ip6_header *)(buffer + 14);
    struct icmp6_header *icmp6 = (struct icmp6_header *)(buffer + 14 + 40);

    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    volatile uint16_t newsum = ntohs(ip6->length) + ip6->proto;

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(&ip6->src[0]), 32);

    /* Sum ICMP6 header and data. */
    icmp6->checksum = 0;
    newsum = chksum(newsum, (uint8_t *)icmp6, ntohs(ip6->length));

    return ~newsum;
}

void EtherSia::icmp6_ns_reply()
{
    struct ip6_header *ip6 = (struct ip6_header *)(buffer + 14);
    struct icmp6_header *icmp6 = (struct icmp6_header *)(buffer + 14 + 40);


    convert_buffer_to_reply();

    // We should now send a neighbor advertisement back to where the neighbor solicication came from.
    ip6->length = ntohs(sizeof(struct icmp6_header));
    icmp6->type = ICMP6_TYPE_NA;
    icmp6->flags = ICMP6_FLAG_S; // Solicited flag.
    icmp6->reserved1 = icmp6->reserved2 = icmp6->reserved3 = 0;
    icmp6->option_type = ICMP6_OPTION_TARGET_LINK_ADDRESS;
    icmp6->option_len = 1;  // Options length, 1 = 8 bytes.
    memcpy(&icmp6->option_mac, &enc_mac_addr, sizeof(enc_mac_addr));
    icmp6->checksum = 0;
    icmp6->checksum = htons(icmp6_chksum());

    enc28j60_send(buffer, 14 + 40 + sizeof(struct icmp6_header));
}

void EtherSia::icmp6_echo_reply()
{
    struct ip6_header *ip6 = (struct ip6_header *)(buffer + 14);
    struct icmp6_header *icmp6 = (struct icmp6_header *)(buffer + 14 + 40);

    convert_buffer_to_reply();

    icmp6->type = ICMP6_TYPE_ECHO_REPLY;
    icmp6->code = 0;
    icmp6->checksum = 0;
    icmp6->checksum = htons(icmp6_chksum());

    enc28j60_send(buffer, 14 + 40 + ntohs(ip6->length));
}

void EtherSia::process_icmp6(uint16_t len)
{
    struct icmp6_header *icmp6 = (struct icmp6_header *)(buffer + 14 + 40);

    Serial.print(F("sizeof(struct icmp6_header)="));
    Serial.println(sizeof(struct icmp6_header), DEC);

    Serial.print(F("icmp6 type="));
    Serial.println(icmp6->type, DEC);

    Serial.print(F("icmp6 code="));
    Serial.println(icmp6->code, DEC);

    Serial.print(F("packet checksum=0x"));
    Serial.println(ntohs(icmp6->checksum), HEX);

    Serial.print(F("calculated checksum=0x"));
    Serial.println(icmp6_chksum(), HEX);


    switch(icmp6->type) {
    case ICMP6_TYPE_NS:
        icmp6_ns_reply();
        break;

    case ICMP6_TYPE_ECHO:
        icmp6_echo_reply();
        break;

    default:
        Serial.print(F("Unknown ICMP6 type="));
        Serial.println(icmp6->type, DEC);
        break;
    }
}
