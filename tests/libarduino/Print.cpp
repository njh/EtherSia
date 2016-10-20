#include <Arduino.h>
#include <stdio.h>

size_t Print::print(const char str[])
{
    unsigned int len = strlen(str);
    for(unsigned int i=0; i<len; i++) {
        write(str[i]);
    }
    return len;
}

size_t Print::print(const __FlashStringHelper* ifsh)
{
    const char* str = reinterpret_cast<const char*>(ifsh);
    return print(str);
}

size_t Print::print(char c)
{
    return write(c);
}

size_t Print::print(int i, int base)
{
    char buf[12] = "\0";
    if (base == DEC) {
        snprintf(buf, sizeof(buf), "%d", i);
    } else if (base == HEX) {
        snprintf(buf, sizeof(buf), "%x", i);
    }
    return print(buf);
}

size_t Print::print(unsigned int i, int base)
{
    char buf[12] = "\0";
    if (base == DEC) {
        snprintf(buf, sizeof(buf), "%u", i);
    } else if (base == HEX) {
        snprintf(buf, sizeof(buf), "%x", i);
    }
    return print(buf);
}

size_t Print::println(const char str[])
{
    return print(str) + println();
}

size_t Print::println(const __FlashStringHelper* ifsh)
{
    return print(ifsh) + println();
}

size_t Print::println(char c)
{
    return print(c) + println();
}

size_t Print::println(int i, int base)
{
    return print(i, base) + println();
}

size_t Print::println(unsigned int i, int base)
{
    return print(i, base) + println();
}

size_t Print::println(void)
{
    return print("\r\n");
}
