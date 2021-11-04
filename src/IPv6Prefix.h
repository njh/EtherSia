/**
 * Header file for the IPv6Prefix class
 * @file IPv6Prefix.h
 */

#ifndef IPv6Prefix_H
#define IPv6Prefix_H

#include <stdint.h>
#include "IPv6Address.h"

/**
 * Class for the storage and manipulation of IPv6 prefixes.
 */
class IPv6Prefix {
private:
    IPv6Address _address;    
    uint8_t _length;

public:
    /**
     * Constructor for a new / all-zero IPv6 /0 prefix
     */
    IPv6Prefix();

    /**
     * Constructor from a IPv6 address and prefix length
     * @param address the prefixes address
     * @param length the prefixes length
     */
     IPv6Prefix(const IPv6Address *address, const uint8_t length);
    
    /**
     * Constructor from a human readable IPv6 prefix string
     * @param prefixstr the prefix to parse
     */
    IPv6Prefix(const char *prefixstr);

    /**
     * Constructor from a human readable IPv6 prefix Flash string (use the F() macro)
     * @param prefixstr the prefix to parse
     */
    IPv6Prefix(const __FlashStringHelper *prefixstr);

    /**
     * Set prefix to the a human readable IPv6 string
     * The prefix string can contain double colon :: notation to save space
     * @param prefixstr an IPv6 prefix as a null-terminated string
     * @return true if the string was parsed successfully
     */
    boolean fromString(const char *prefixstr);

    /**
     * Set prefix to the a human readable IPv6 Flash string (use the F() macro)
     * The prefix string can contain double colon :: notation to save space
     * @param prefixstr an IPv6 prefix as a null-terminated string
     * @return true if the string was parsed successfully
     */
    boolean fromString(const __FlashStringHelper *prefixstr);

    /**
     * Returns the prefix address
     * @return the prefix address
     */
     const IPv6Address* address() const;

    /**
     * Returns the prefix length
     * @return the prefix length
     */
    uint8_t getLength() const ;

    /**
     * Sets the prefix length
     * @param prefix_length the new prefix length
     * @return returns true if new length is valid, false if otherwise
     */
    boolean setLength(uint8_t prefix_length);

    /**
     * Checks if another IPv6 Prefix is contained by the prefix
     * @param prefix the prefix to be checked
     * @return true if prefix is contained, false if not
     */
    boolean contains(const IPv6Prefix *prefix);

    /**
     * Cast the IPv6 prefix to an array of octets.
     */
    operator uint8_t*();

    /**
     * Set the prefix to all-zeros (::/0)
     */
    void setZero();

    /**
     * Check if the prefix equals another IPv6 prefix.
     * @param prefix the second prefix to compare to
     * @return true if the two prefixes are the same
     */
    boolean operator==(const IPv6Prefix& prefix) const;

    /**
     * Check if the prefix is not equal to another IPv6 prefix.
     * @param prefix the second prefix to compare to
     * @return true if the two prefixes are the same
     */
    boolean operator!=(const IPv6Prefix& prefix) const;

    /**
     * Print a IPv6 prefix to a stream as a human readable string.
     * @param print The stream to print to (defaults to Serial)
     */
    void print(Print &print=Serial) const;

    /**
     * Print a IPv6 prefix to a stream with line ending.
     * @param print The stream to print to (defaults to Serial)
     */
    void println(Print &print=Serial) const;

} __attribute__((__packed__));

 #endif
 