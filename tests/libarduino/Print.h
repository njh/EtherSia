#ifndef Print_h
#define Print_h

#include "Progmem.h"

#include <stdint.h>
#include <string.h>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print {

public:
    size_t print(const char str[]);
    size_t print(const __FlashStringHelper* ifsh);
    size_t print(char c);
    size_t print(int i, int base = DEC);
    size_t print(unsigned int i, int base = DEC);

    size_t println(const char str[]);
    size_t println(const __FlashStringHelper* ifsh);
    size_t println(char c);
    size_t println(int i, int = DEC);
    size_t println(unsigned int i, int base = DEC);
    size_t println(void);

    virtual size_t write(uint8_t chr) = 0;
};

#endif
