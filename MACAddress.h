#ifndef MACAddress_H
#define MACAddress_H

#include <Printable.h>
#include <stdint.h>


class MACAddress : public Printable {
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

    virtual size_t printTo(Print& p) const;

    static void printPaddedHex(Print& p, uint8_t byte);

} __attribute__((__packed__));

#endif
