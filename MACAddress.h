#ifndef MACAddress_H
#define MACAddress_H

#include <stdint.h>


class MACAddress {
private:
    uint8_t _address[6];

public:
    MACAddress();
    MACAddress(const char *macstr);
    MACAddress(uint8_t one, uint8_t two, uint8_t three, uint8_t four, uint8_t five, uint8_t six);

    bool fromString(const char *macstr);

    uint8_t* getPtr();
    operator uint8_t*() { return getPtr(); };

    uint8_t operator[](int index) const { return _address[index]; };
    uint8_t& operator[](int index) { return _address[index]; };

    void print(Print &print=Serial);
    void println(Print &print=Serial);

} __attribute__((__packed__));

#endif
