#ifndef Buffer_h
#define Buffer_h

#include "Print.h"


class Buffer : public Print {

public:
    Buffer(size_t size=2048);
    ~Buffer();

    virtual size_t write(uint8_t chr);

    uint16_t size() { return _pos; };

    operator uint8_t*() { return (uint8_t*)_buffer; };
    operator char*() { return _buffer; };

protected:
    char *_buffer;
    size_t _pos;
    size_t _allocated;
};

#endif
