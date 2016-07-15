#include <Arduino.h>
#include <stdio.h>

size_t Print::print(const char str[])
{
    return printf("%s", str);
}

size_t Print::print(char c)
{
    return printf("%c", c);
}

size_t Print::print(unsigned char i, int base)
{
    return printf("%d:%d", i, base);
}

size_t Print::print(int i, int base)
{
    return printf("%d:%d", i, base);
}

size_t Print::print(unsigned int i, int base)
{
    return printf("%d:%d", i, base);
}

size_t Print::print(long i, int base)
{
    return printf("%ld:%d", i, base);
}

size_t Print::print(unsigned long i, int base)
{
    return printf("%lu:%d", i, base);
}

size_t Print::println(const char str[])
{
    return printf("%s\n", str);
}

size_t Print::println(char c)
{
    return printf("%c\n", c);
}

size_t Print::println(unsigned char i, int base)
{
    return printf("%d:%d\n", i, base);
}

size_t Print::println(int i, int base)
{
    return printf("%d:%d\n", i, base);
}

size_t Print::println(unsigned int i, int base)
{
    return printf("%u:%d\n", i, base);
}

size_t Print::println(long i, int base)
{
    return printf("%ld:%d\n", i, base);
}

size_t Print::println(unsigned long i, int base)
{
    return printf("%lu:%d\n", i, base);
}

size_t Print::println(void)
{
    return printf("\n");
}
