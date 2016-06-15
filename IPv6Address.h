#ifndef IPv6Address_H
#define IPv6Address_H

#include <stdint.h>


class IPv6Address {
private:
    uint8_t _address[16];

public:
    IPv6Address();
    IPv6Address(const uint8_t *addr);

    uint8_t* getPtr();
    operator uint8_t*() { return getPtr(); };
    bool operator==(const IPv6Address& addr) const;

    void setLinkLocalPrefix();
    void setEui64(const MACAddress *macaddr);

    bool isMulticast() const;

    void print(Print &print=Serial) const;
    void println(Print &print=Serial) const;

} __attribute__((__packed__));

#endif
