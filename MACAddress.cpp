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

MACAddress::MACAddress(const char *macstr)
{
    fromString(macstr);
}

uint8_t* MACAddress::getPtr()
{
    return _address;
}

static int8_t asciiToHex(uint8_t c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    } else {
        return -1;
    }
}

bool MACAddress::fromString(const char *macstr)
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

void MACAddress::print(Print &p)
{
    for (uint8_t i = 0; i < 6; ++i) {
        printPaddedHex(_address[i], p);
        if (i < 5)
            p.print(':');
    }
}

void MACAddress::println(Print &p)
{
    this->print(p);
    p.println();
}
