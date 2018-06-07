/**
 * Header file for the IPv6Address class
 * @file IPv6Address.h
 */

#ifndef IPv6Address_H
#define IPv6Address_H

#include <stdint.h>
#include "MACAddress.h"

/** The maximum string length of an IPv6 address */
#define MAX_IPV6_ADDRESS_STR_LEN  39


/** A enumeration of IPv6 address types */
enum IPv6AddressType {
    ADDRESS_TYPE_LINK_LOCAL = 1,  /**< Link-local address type */
    ADDRESS_TYPE_GLOBAL,          /**< Global address type */
    ADDRESS_TYPE_MULTICAST        /**< Multicast address type */
};


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
     * @param address an array of bytes
     */
    IPv6Address(const uint8_t *address);

    /**
     * Constructor from a human readable IPv6 address string
     * @param addrstr the address to parse
     */
    IPv6Address(const char *addrstr);

    /**
     * Constructor from a human readable IPv6 address Flash string (use the F() macro)
     * @param addrstr the address to parse
     */
    IPv6Address(const __FlashStringHelper *addrstr);

    /**
     * Set address to the a human readable IPv6 string
     * The address string can contain double colon :: notation to save space
     * @param addrstr an IPv6 address as a null-terminated string
     * @return true if the string was parsed successfully
     */
    boolean fromString(const char *addrstr);

    /**
     * Set address to the a human readable IPv6 Flash string (use the F() macro)
     * The address string can contain double colon :: notation to save space
     * @param addrstr an IPv6 address as a null-terminated string
     * @return true if the string was parsed successfully
     */
    boolean fromString(const __FlashStringHelper *addrstr);

    /**
     * Get the IPv6 address type
     * @return a value from the IPv6AddressType enumeration
     */
    uint8_t type();

    /**
     * Cast the IPv6 address to an array of octets.
     */
    operator uint8_t*();

    /**
     * Check if the address equals another IPv6 address.
     * @param address the second address to compare to
     * @return true if the two addresses are the same
     */
    boolean operator==(const IPv6Address& address) const;

    /**
     * Check if the address is not equal to another IPv6 address.
     * @param address the second address to compare to
     * @return true if the two addresses are the same
     */
    boolean operator!=(const IPv6Address& address) const;

    /**
     * Set the first two bytes to the link-local prefix (FE80:)
     * Note this only sets the first 64-bits of the address.
     * The last 64-bits are left unmodified.
     */
    void setLinkLocalPrefix();

    /**
     * Set address to multicast address for all nodes on the local network segment (FF02::1)
     */
    void setLinkLocalAllNodes();

    /**
     * Check if multicast address for all nodes on the local network segment (FF02::1)
     *
     * @return true if address if for the Link Local all nodes address
     */
    boolean isLinkLocalAllNodes() const;

    /**
     * Set address to multicast address for all routers on the local network segment (FF02::2)
     */
    void setLinkLocalAllRouters();

    /**
     * Check if multicast address for all routers on the local network segment (FF02::2)
     *
     * @return true if address if for the Link Local all routers address
     */
    boolean isLinkLocalAllRouters() const;

    /**
     * Set the last 64-bits of the IPv6 address to a EUI-64 based on a 48-bit MAC Address
     * Note this only sets the last 64-bits of the address.
     * The first 64-bits are left unmodified.
     * @param macAddress The MAC address
     */
    void setEui64(const MACAddress &macAddress);

    /**
     * Set the address to a Solicited Node Multicast Address
     * For example the multicast address for fe80::2aa:ff:fe28:9c5a would be ff02::1:ff28:9c5a
     * @param address The IPv6 unicast address
     */
    void setSolicitedNodeMulticastAddress(const IPv6Address &address);

    /**
     * Check if address is a specific node multicast address
     *
     * For example the multicast address for fe80::2aa:ff:fe28:9c5a would be ff02::1:ff28:9c5a
     *
     * @param address The IPv6 unicast address to calculate the multicast address from
     * @return true if address if the specific node multicast address
     */
    boolean isSolicitedNodeMulticastAddress(const IPv6Address &address) const;

    /**
     * Set the address to all-zeros (::)
     */
    void setZero();

    /**
     * Check if the address is a multicast address (starts FF)
     * @return true if the address is a multicast address
     */
    boolean isMulticast() const;

    /**
     * Check if the address is a link-local address (starts FE80)
     * @return true if the address is a link-local address
     */
    boolean isLinkLocal() const;

    /**
     * Check if the address is empty / all-zeros (::)
     * @return true if the address is empty / all-zeros (::)
     */
    boolean isZero() const;

    /**
     * Returns true if two addresses are in the same subnet
     *
     * @note assumes that local subnet is /64 (which is an assumption elsewhere too)
     * @param address the address to
     * @return true if the addresses are in the same subnet
     */
    boolean inSameSubnet(const IPv6Address& address) const;

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

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(IPv6Address) == 16, "Size is not correct");

#endif
