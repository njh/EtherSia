#include "Arduino.h"


Buffer::Buffer(size_t size)
{
    _buffer = (char*)malloc(size);
    memset(_buffer, 0, size);
    _allocated = size;
    _pos = 0;
}

Buffer::~Buffer()
{
    free(_buffer);
}

size_t Buffer::write(uint8_t chr)
{
    _buffer[_pos++] = chr;
    return 1;
}
