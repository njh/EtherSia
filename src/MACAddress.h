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
     * Get a pointer to a 6-byte array of octets.
     */
    uint8_t* getPtr();

    /**
     * Cast the MAC address to an array of octets.
     */
    operator uint8_t*() { return getPtr(); };

    /**
     * Calculate the multicast MAC address for an IPv6 address.
     * @param addr An IPv6 address as an array of 16-bytes
     */
    void setIPv6Multicast(const uint8_t* addr);

    /**
     * Get an individual octet from the MAC address.
     * Indexed from 0 to 5.
     */
    uint8_t operator[](int index) const { return _address[index]; };

    /**
     * Get an individual octet from the MAC address.
     * Indexed from 0 to 5.
     */
    uint8_t& operator[](int index) { return _address[index]; };

    /**
     * Read a EUI-48 MAC address from a I2C EEPROM device
     *
     * Default settings are for an AT24MAC402 EEPROM.
     *
     * @param address the I2C address (default 0x58).
     * @param location the memory location in the EEPROM (default 0x9A)
     */
    void readI2C(uint8_t address=0x58, uint8_t location=0x9A);

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

#endif
