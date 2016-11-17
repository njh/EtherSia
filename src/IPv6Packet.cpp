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
    _etherType = ntohs(ETHER_TYPE_IPV6);
    _ver_tc = 0x60;  // Version and Traffic Class
    _tc_fl = 0x00;   // Traffic Class and Flow Label
    _flowLabel = 0;  // Flow Label
    _hopLimit = IP6_DEFAULT_HOP_LIMIT;
}

uint8_t* IPv6Packet::payload()
{
    return (uint8_t *)(this) + sizeof(IPv6Packet);
}

boolean IPv6Packet::isValid()
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

void IPv6Packet::invalidate()
{
    this->_etherType = 0;
}

uint16_t IPv6Packet::etherType() const
{
    return ntohs(_etherType);
}

uint8_t IPv6Packet::version() const
{
    return (_ver_tc & 0xF0) >> 4;
}

uint16_t IPv6Packet::length() const
{
    return ETHER_HEADER_LEN + IP6_HEADER_LEN + payloadLength();
}

uint16_t IPv6Packet::payloadLength() const
{
    return ntohs(_length);
}

void IPv6Packet::setPayloadLength(uint16_t length)
{
    _length = htons(length);
}

uint8_t IPv6Packet::protocol()
{
    return _protocol;
}

void IPv6Packet::setProtocol(uint8_t protocol)
{
    _protocol = protocol;
}

uint8_t IPv6Packet::hopLimit()
{
    return _hopLimit;
}

void IPv6Packet::setHopLimit(uint8_t hopLimit)
{
    _hopLimit = hopLimit;
}

MACAddress& IPv6Packet::etherSource()
{
    return _etherSource;
}

void IPv6Packet::setEtherSource(MACAddress& address)
{
    _etherSource = address;
}

MACAddress& IPv6Packet::etherDestination()
{
    return _etherDestination;
}

void IPv6Packet::setEtherDestination(MACAddress& address)
{
    _etherDestination = address;
}

IPv6Address& IPv6Packet::source()
{
    return _source;
}

void IPv6Packet::setSource(IPv6Address& address)
{
    _source = address;
}

IPv6Address& IPv6Packet::destination()
{
    return _destination;
}

void IPv6Packet::setDestination(IPv6Address& address)
{
    _destination = address;
}


// This function is derived from Contiki's uip6.c / upper_layer_chksum()
uint16_t IPv6Packet::calculateChecksum()
{
    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    volatile uint16_t newsum = payloadLength() + protocol();

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(source()), 16);
    newsum = chksum(newsum, (uint8_t *)(destination()), 16);

    /* Sum the payload header and data */
    newsum = chksum(newsum, payload(), payloadLength());

    return ~newsum;
}
