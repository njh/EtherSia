#include <Arduino.h>
#include "MACAddress.h"
#include "IPv6Address.h"
#include "util.h"

IPv6Address::IPv6Address()
{
    setZero();
}

IPv6Address::IPv6Address(const char *addrstr)
{
    fromString(addrstr);
}

void IPv6Address::setZero()
{
    memset(_address, 0, sizeof(_address));
}

void IPv6Address::setLinkLocalPrefix()
{
    _address[0] = 0xfe;
    _address[1] = 0x80;
}

boolean IPv6Address::isMulticast() const
{
    return _address[0] == 0xFF;
}

boolean IPv6Address::isLinkLocal() const
{
    return _address[0] == 0xfe && _address[1] == 0x80;
}

boolean IPv6Address::isZero() const
{
    for(uint8_t i=0; i < 16; i++) {
        if (_address[i] != 0x00)
            return 0;
    }

    return 1;
}

void IPv6Address::setEui64(const MACAddress *macaddr)
{
    _address[8] = (*macaddr)[0] ^ 0x02;
    _address[9] = (*macaddr)[1];
    _address[10] = (*macaddr)[2];
    _address[11] = 0xff;
    _address[12] = 0xfe;
    _address[13] = (*macaddr)[3];
    _address[14] = (*macaddr)[4];
    _address[15] = (*macaddr)[5];
}

void IPv6Address::setLinkLocalAllNodes()
{
    setZero();
    _address[0] = 0xFF;
    _address[1] = 0x02;
    _address[15] = 0x01;
}

void IPv6Address::setLinkLocalAllRouters()
{
    setZero();
    _address[0] = 0xFF;
    _address[1] = 0x02;
    _address[15] = 0x02;
}

// See RFC4291 section 2.7.1.
void IPv6Address::setSolicitedNodeMulticastAddress(const IPv6Address *addr)
{
    setZero();
    _address[0] = 0xFF;
    _address[1] = 0x02;
    _address[11] = 0x01;
    _address[12] = 0xFF;
    _address[13] = addr->_address[13];
    _address[14] = addr->_address[14];
    _address[15] = addr->_address[15];
}

uint8_t* IPv6Address::getPtr()
{
    return _address;
}

boolean IPv6Address::operator==(const IPv6Address& addr) const
{
    return memcmp(_address, addr._address, sizeof(_address)) == 0;
}

boolean IPv6Address::fromString(const char *addrstr)
{
    uint16_t accumulator = 0;
    uint8_t colon_count = 0;
    uint8_t pos = 0;

    memset(_address, 0, sizeof(_address));

    // Step 1: look for position of ::, and count colons after it
    for(uint8_t i=1; i <= MAX_IPV6_ADDRESS_STR_LEN; i++) {
        if (addrstr[i] == ':') {
            if (addrstr[i-1] == ':') {
                // Double colon!
                colon_count = 14;
            } else if (colon_count) {
                // Count backwards the number of colons after the ::
                colon_count -= 2;
            }
        } else if (addrstr[i] == '\0') {
            break;
        }
    }

    // Step 2: convert from ascii to binary
    for(uint8_t i=0; i <= MAX_IPV6_ADDRESS_STR_LEN && pos < 16; i++) {
        if (addrstr[i] == ':' || addrstr[i] == '\0') {
            _address[pos] = accumulator >> 8;
            _address[pos+1] = accumulator;
            accumulator = 0;

            if (colon_count && i && addrstr[i-1] == ':') {
                pos = colon_count;
            } else {
                pos += 2;
            }
        } else {
            int8_t val = asciiToHex(addrstr[i]);
            if (val == -1) {
                // Not hex or colon: fail
                return 0;
            } else {
                accumulator <<= 4;
                accumulator |= val;
            }
        }

        if (addrstr[i] == '\0')
            break;
    }

    // Success
    return 1;
}

void IPv6Address::print(Print &p) const
{
    for (byte i = 0; i < 16; ++i) {
        printPaddedHex(_address[i], p);
        if (i % 2 == 1 && i < 15)
            p.print(':');
    }
}

void IPv6Address::println(Print &p) const
{
    this->print(p);
    p.println();
}
