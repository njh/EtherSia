#include <Arduino.h>

#include "EtherSia.h"
#include "IPv6Packet.h"
#include "util.h"

IPv6Packet::IPv6Packet()
{
    memset(this, 0, sizeof(IPv6Packet));
    this->ver_tc = 0x60;
    this->etherType = ntohs(ETHER_TYPE_IPV6);
    this->hopLimit = IP6_DEFAULT_HOP_LIMIT;
}

uint8_t* IPv6Packet::payload()
{
    return (uint8_t *)(this + ETHER_HEADER_LEN + IP6_HEADER_LEN);
}

// This function is derived from Contiki's uip6.c / upper_layer_chksum()
uint16_t IPv6Packet::calculateChecksum()
{
    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    volatile uint16_t newsum = ntohs(this->length) + this->proto;

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(this->src), 32);

    /* Sum the payload header and data */
    newsum = chksum(newsum, this->payload(), ntohs(this->length));

    return ~newsum;
}
