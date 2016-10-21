/**
 * Utility functions that don't belong to a specific class
 * @file util.h
 */

#include <Arduino.h>

#ifndef ETHERSIA_UTIL
#define ETHERSIA_UTIL

/**
 * Convert an ASCII hex character to its integer value
 *
 * @param c The character (0-9, a-f or A-F)
 * @return The integer value or -1 if it is invalid
 */
int8_t asciiToHex(char c);

/**
 * Check if a string contains one or more colons
 *
 * @param str The string to check
 * @return True if the string contains at least one colon
 */
boolean containsColon(const char *str);

/**
 * Print a 2-byte human readable hex value for an 8-bit integer
 *
 * @param byte The value to print (in range 0x00 to 0xFF)
 * @param p The stream to print to (default Serial)
 */
void printPaddedHex(uint8_t byte, Print &p=Serial);

/**
 * Print a 4-byte human readable hex value for an 16-bit integer
 *
 * @param word The value to print (in range 0x0000 to 0xFFFF)
 * @param p The stream to print to (default Serial)
 */
void printPaddedHex16(uint16_t word, Print &p=Serial);

/**
 * Print an array of bytes in hex, as rows of 16 bytes
 *
 * @param bytes the array of bytes
 * @param len the number bytes in the array
 * @param p The stream to print to (default Serial)
 */
void printHexDump(const uint8_t bytes[], uint16_t len, Print &p=Serial);

/**
 * Calculate a IP type 16-bit checksum for a buffer
 *
 * @param sum The current sum accumulator (or 0 for first call)
 * @param data A pointer to the data buffer to calculate checksum for
 * @param len The length of the data (in bytes) to perform checksum on
 * @return The calculated checksum
 */
uint16_t chksum(uint16_t sum, const uint8_t *data, uint16_t len);

/**
 * Macro to make it easy to define AVR flash strings as static members of a class
 *
 * @param klass The name of the class
 * @param var The name of the member variable
 * @param str The string literal
 */
#define FlashStringMaker(klass, var, str) \
    static const char _fsm_##var[] PROGMEM = str; \
    const __FlashStringHelper* klass::var = reinterpret_cast<const __FlashStringHelper *>(_fsm_##var)

#endif
