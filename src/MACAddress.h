/**
 * Header file for the MACAddress class
 * @file MACAddress.h
 */

#ifndef MACAddress_H
#define MACAddress_H

#include <stdint.h>


/**
 * Class for the storage and manipulation of 48-bit Ethernet addresses.
 */
class MACAddress {
private:
    uint8_t _address[6];

public:
    /**
     * Constructor for a new / all-zero MAC address (00:00:00:00:00:00)
     */
    MACAddress();

    /**
     * Constructor for MAC address from a string.
     * @param macstr a human readable string containing a 17 character MAC address
     */
    MACAddress(const char *macstr);

    /**
     * Constructor for MAC address from an array of 6 bytes
     * @param macaddr a 48-bit (6 byte) MAC address
     */
    MACAddress(const byte macaddr[6]);

    /**
     * Constructor for MAC address from 6-octets.
     * @param one The first octet of the address
     * @param two The second octet of the address
     * @param three The third octet of the address
     * @param four The fourth octet of the address
     * @param five The fifth octet of the address
     * @param six The sixth octet of the address
     */
    MACAddress(uint8_t one, uint8_t two, uint8_t three, uint8_t four, uint8_t five, uint8_t six);

    /**
     * Parse a human readable MAC address into a MACAddress object.
     * @param macstr a human readable string containing a 17 character MAC address
     * @return true if successful, false if parsing failed
     */
    boolean fromString(const char *macstr);

    /**
     * Cast the MAC address to an array of octets.
     */
    operator uint8_t*();

    /**
     * Calculate the multicast MAC address for an IPv6 address.
     * @param address An IPv6 address as an array of 16-bytes
     */
    void setIPv6Multicast(const uint8_t* address);

    /**
     * Get an individual octet from the MAC address.
     * Indexed from 0 to 5.
     * @param index The byte number from the address to get
     * @return The requested byte from the address
     */
    uint8_t operator[](int index) const;

    /**
     * Print a MAC address to a stream as a human readable string.
     * @param print The stream to print to (defaults to Serial)
     */
    void print(Print &print=Serial) const;

    /**
     * Print a MAC address to a stream with line ending.
     * @param print The stream to print to (defaults to Serial)
     */
    void println(Print &print=Serial) const;

} __attribute__((__packed__));

/* Verify that compiler gets the structure size correct */
static_assert(sizeof(MACAddress) == 6, "Size is not correct");

#endif
