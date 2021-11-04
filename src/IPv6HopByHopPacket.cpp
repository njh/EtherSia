#include <Arduino.h>

#include "EtherSia.h"
#include "IPv6HopByHopPacket.h"

IPv6HopByHopPacket::IPv6HopByHopPacket()
{
    memset(this, 0, sizeof(IPv6HopByHopPacket));
    init();
}

void IPv6HopByHopPacket::setNextHeader(uint8_t header) {
    _next_header = header;
}

uint8_t IPv6HopByHopPacket::nextHeader() {
    return _next_header;
}

void IPv6HopByHopPacket::setHopByHopLength(uint8_t length) {
    _hbh_length = length;
}

uint8_t IPv6HopByHopPacket::hopByHopLength() {
    return _hbh_length;
}

uint8_t IPv6HopByHopPacket::optionType() {
    return hbh_option[0];
}

void IPv6HopByHopPacket::setMLDRouterAlert() {
    _router_alert.option_type = 5;
    _router_alert.length = 2;
    _router_alert.alert_type = 0;
    _router_alert.padding = 0;
}

uint8_t* IPv6HopByHopPacket::payload()
{
    return (uint8_t *)(this) + sizeof(IPv6HopByHopPacket);
}

uint16_t IPv6HopByHopPacket::calculateChecksum()
{
    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    volatile uint16_t newsum = (payloadLength() - 8) + nextHeader();

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(source()), 16);
    newsum = chksum(newsum, (uint8_t *)(destination()), 16);

    /* Sum the payload header and data */
    newsum = chksum(newsum, payload(), (payloadLength() - 8));

    return ~newsum;
}

boolean IPv6HopByHopPacket::isValid()
{
    if (this->_etherType != ntohs(ETHER_TYPE_IPV6)) {
        return false;
    }

    // Check the version header
    if (this->version() != 6) {
        return false;
    }

    // Verify the packet checksum (it should add up to 0)
    if (calculateChecksum() != 0) {
        return false;
    }

    return true;
}
