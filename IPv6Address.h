#ifndef IPv6Address_H
#define IPv6Address_H

#include <stdint.h>


#define MAX_IPV6_ADDRESS_STR_LEN  39


/**
 * Class for the storage and manipulation of IPv6 addresses.
 */
class IPv6Address {
private:
    uint8_t _address[16];

public:
    /**
     * Constructor for a new / all-zero IPv6 address (::)
     */
    IPv6Address();

    /**
     * Constructor from a 128 bit (16-byte) array of bytes
     * @param addr an array of bytes
     */
    IPv6Address(const uint8_t *addr);
    IPv6Address(const char *addrstr);

    bool fromString(const char *addrstr);

    /**
     * Get a pointer to a 16-byte array of octets.
     */
    uint8_t* getPtr();

    /**
     * Cast the IPv6 address to an array of octets.
     */
    operator uint8_t*() { return getPtr(); };

    /**
     * Check if the address equals another IPv6 address.
     */
    bool operator==(const IPv6Address& addr) const;

    /**
     * Set the first two bytes to the link-local prefix (FE80:)
     */
    void setLinkLocalPrefix();

    /**
     * Set address to multicast address for all nodes on the local network segment (FF02::1)
     */
    void setLinkLocalAllNodes();

    /**
     * Set address to multicast address for all routers on the local network segment (FF02::2)
     */
    void setLinkLocalAllRouters();

    /**
     * Set the last 64-bits of the IPv6 address to a EUI-64 based on a 48-bit MAC Address
     * @param macaddr The MAC address
     */
    void setEui64(const MACAddress *macaddr);

    /**
     * Set the address to a Solicited Node Multicast Address
     * For example the multicast address for fe80::2aa:ff:fe28:9c5a would be ff02::1:ff28:9c5a
     * @param addr The IPv6 unicast address
     */
    void setSolicitedNodeMulticastAddress(const IPv6Address *addr);

    /**
     * Set the address to all-zeros (::)
     */
    void setZero();

    /**
     * Returns true if the address is a multicast address (starts FF)
     */
    bool isMulticast() const;

    /**
     * Returns true if the address is a link-local address (starts FE80)
     */
    bool isLinkLocal() const;

    /**
     * Returns true if the address is empty / all-zeros (::)
     */
    bool isZero() const;

    /**
     * Print a IPv6 address to a stream as a human readable string.
     * @param print The stream to print to (defaults to Serial)
     */
    void print(Print &print=Serial) const;

    /**
     * Print a IPv6 address to a stream with line ending.
     * @param print The stream to print to (defaults to Serial)
     */
    void println(Print &print=Serial) const;

} __attribute__((__packed__));

#endif
