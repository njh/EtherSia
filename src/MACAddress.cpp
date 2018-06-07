#include <Arduino.h>
#include "MACAddress.h"
#include "util.h"

MACAddress::MACAddress()
{
    memset(_address, 0, sizeof(_address));
}

MACAddress::MACAddress(uint8_t one, uint8_t two, uint8_t three, uint8_t four, uint8_t five, uint8_t six)
{
    _address[0] = one;
    _address[1] = two;
    _address[2] = three;
    _address[3] = four;
    _address[4] = five;
    _address[5] = six;
}

MACAddress::MACAddress(const byte macaddr[6])
{
    memcpy(_address, macaddr, sizeof(_address));
}

MACAddress::MACAddress(const char *macstr)
{
    fromString(macstr);
}

MACAddress::MACAddress(const __FlashStringHelper *macstr)
{
    fromString(macstr);
}

MACAddress::operator uint8_t*()
{
    return _address;
}

boolean MACAddress::operator==(const MACAddress& address) const
{
    return memcmp(_address, address._address, sizeof(_address)) == 0;
}

boolean MACAddress::operator!=(const MACAddress& address) const
{
    return !(*this == address);
}

// See RFC2464 section 7
void MACAddress::setIPv6Multicast(const uint8_t *address)
{
    _address[0] = 0x33;
    _address[1] = 0x33;
    _address[2] = address[12];
    _address[3] = address[13];
    _address[4] = address[14];
    _address[5] = address[15];
}

boolean MACAddress::isIPv6Multicast()
{
    return _address[0] == 0x33 && _address[1] == 0x33;
}

boolean MACAddress::fromString(const char *macstr)
{
    uint8_t pos=0;

    for(uint8_t strpos=0; strpos < 17; strpos++) {
        uint8_t mod = strpos%3;
        if (mod == 2) {
            // should always be a colon or dash in the third position
            if (macstr[strpos] == ':' || macstr[strpos] == '-') {
                pos++;
            } else {
                // Fail
                return 0;
            }
        } else {
            int8_t val = asciiToHex(macstr[strpos]);
            if (val == -1) {
                // Fail
                return 0;
            } else if (mod == 0) {
                _address[pos] = val << 4;
            } else if (mod == 1) {
                _address[pos] |= val;
            }
        }
    }

    // Success
    return 1;
}

boolean MACAddress::fromString(const __FlashStringHelper *macstr)
{
    char ramStr[18];
    // Copy the string from flash program memory into RAM
    strcpy_P(ramStr, (const char*)macstr);
    return fromString(ramStr);
}

uint8_t MACAddress::operator[](int index) const
{
    return _address[index];
}

void MACAddress::print(Print &p) const
{
    for (uint8_t i = 0; i < 6; ++i) {
        printPaddedHex(_address[i], p);
        if (i < 5)
            p.print(':');
    }
}

void MACAddress::println(Print &p) const
{
    this->print(p);
    p.println();
}
