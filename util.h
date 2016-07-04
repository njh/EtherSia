#include <Arduino.h>

int8_t asciiToHex(char c);
void printPaddedHex(uint8_t byte, Print &p=Serial);
uint16_t chksum(uint16_t sum, const uint8_t *data, uint16_t len);
