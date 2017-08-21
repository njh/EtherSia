#include <Arduino.h>
#include "MACAddress.h"
#include "IPv6Address.h"
#include "IPv6Prefix.h"
#include "util.h"

IPv6Prefix::IPv6Prefix()
{
    setZero();
}

IPv6Prefix::IPv6Prefix(const IPv6Address *address, const uint8_t length) {
    memcpy(_address, address, sizeof(IPv6Address));
    _length = length;
}

IPv6Prefix::IPv6Prefix(const char *prefixstr)
{
    fromString(prefixstr);
}

IPv6Prefix::IPv6Prefix(const __FlashStringHelper *prefixstr)
{
    fromString(prefixstr);
}

boolean IPv6Prefix::fromString(const char *prefixstr)
{
    setZero();

    bool size_reached = false;
    int8_t address_chars_read = 0;
    int8_t prefix_chars_read = 0;
    uint8_t prefix_chars[3];
    char address[45];
    for (uint8_t i = 0; i < strlen(prefixstr); i++) {
        if (address_chars_read > 45) {
            // Aboort if more than 45 Addres characters are read
            return false;
        } else if (!size_reached && prefixstr[i] == '/') {
            // Begin to read prefix if seperator is read
            size_reached = true;
            // Zero address memory and write the read address to it
            memset(address, 0, 45);
            memcpy((void *) address, prefixstr, address_chars_read);
            // Abort if address is invalid
            if (_address.fromString(address) != true) return false;
        } else if (!size_reached) {
            // Increment address character counter
            address_chars_read++;
        } else if (size_reached) {
            if(prefixstr[i] >= 48 && prefixstr[i] <= 57 && prefix_chars_read < 3) {
                // Read prefix size chars
                prefix_chars[prefix_chars_read] = prefixstr[i] - 48;
                prefix_chars_read++;
            } else {
                // Abort on invalid characters and more than 4 prefix characters
                return false;
            }
        }
    }
    uint8_t multiplier = 1;
    uint8_t length = 0;
    for (int8_t i = prefix_chars_read; i > 0; i--) {
        // Calculate Prefix size from characters
        length += prefix_chars[i - 1] * multiplier;
        multiplier = multiplier * 10;
    }
    return setLength(length);
}

boolean IPv6Prefix::fromString(const __FlashStringHelper *prefixstr)
{
    char ramStr[44];
    // Copy the string from flash program memory into RAM
    strcpy_P(ramStr, (const char*)prefixstr);
    return fromString(ramStr);
}

const IPv6Address* IPv6Prefix::address() const
{
    return &_address;
}

uint8_t IPv6Prefix::getLength() const 
{
    return _length;
}

boolean IPv6Prefix::setLength(uint8_t prefix_length) 
{
    // Abort and return false if size is greater than 128
    if(prefix_length > 128) return false;
    // save it otherwise
    _length = prefix_length;
    
    // Bits to mask (intial value == prefix_length)
    uint8_t to_mask = this->getLength();
    for(uint8_t i=0; i<16; i++) {
        uint8_t mask = 0xFF;
        if(to_mask < 8 ) {
            uint8_t bit_remove = 8 - to_mask;
            for(uint8_t j = 0; j < bit_remove; j++) {
                mask = mask - (1 << j);
            }
            to_mask = 0;
        } else {
            to_mask = to_mask - 8;
        }
        // mask address
        this->_address[i] = this->_address[i] & mask;
    }

    return true;
}

boolean IPv6Prefix::contains(const IPv6Prefix *prefix) 
{
    if(prefix->getLength() < this->getLength()) {
        // If candidate prefix is smaller than current prefix, return false
        return false;
    }
    
    IPv6Prefix candidate;
    memcpy(candidate, prefix, sizeof(IPv6Prefix));
    candidate.setLength(this->getLength());
    return (candidate == *(this));
}

IPv6Prefix::operator uint8_t*()
{
    return _address;
}

void IPv6Prefix::setZero()
{
    _address.setZero();
    _length = 0;
}

boolean IPv6Prefix::operator==(const IPv6Prefix& prefix) const
{
    return *(address()) == *(prefix.address()) && getLength() == prefix.getLength();
}

boolean IPv6Prefix::operator!=(const IPv6Prefix& address) const
{
    return !(*this == address);
}

void IPv6Prefix::print(Print &p) const
{
    _address.print(p);
    p.print("/");
    p.print(this->_length);
}

void IPv6Prefix::println(Print &p) const
{
    this->print(p);
    p.println();
}
