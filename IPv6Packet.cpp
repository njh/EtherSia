#include <Arduino.h>

#include "EtherSia.h"
#include "IPv6Packet.h"
#include "util.h"

IPv6Packet::IPv6Packet()
{
    memset(this, 0, sizeof(IPv6Packet));
    init();
}

void IPv6Packet::init()
{
    this->etherType = ntohs(ETHER_TYPE_IPV6);
    this->ver_tc = 0x60;  // Version and Traffic Class
    this->tc_fl = 0x00;   // Traffic Class and Flow Label
    this->flowLabel = 0;  // Flow Label
    this->hopLimit = IP6_DEFAULT_HOP_LIMIT;
}

uint8_t* IPv6Packet::payload()
{
    return (uint8_t *)(this) + sizeof(IPv6Packet);
}

bool IPv6Packet::isValid()
{
    if (this->etherType != ntohs(ETHER_TYPE_IPV6)) {
        return false;
    }

    // Check the version header
    if ((this->ver_tc & 0x60) != 0x60) {
        Serial.println("NOT 6");
        return false;
    }

    return true;
}

// This function is derived from Contiki's uip6.c / upper_layer_chksum()
uint16_t IPv6Packet::calculateChecksum()
{
    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    uint16_t length = ntohs(this->length);
    volatile uint16_t newsum = length + this->proto;

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(this->src), 32);

    /* Sum the payload header and data */
    newsum = chksum(newsum, this->payload(), length);

    return ~newsum;
}
