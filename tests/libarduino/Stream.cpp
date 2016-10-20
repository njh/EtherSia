#include "Stream.h"

#include <stdio.h>

Stream Serial;
Stream Stdout;

void Stream::begin(int)
{
}

size_t Stream::write(uint8_t chr)
{
    return fwrite(&chr, 1, 1, stdout);
}
