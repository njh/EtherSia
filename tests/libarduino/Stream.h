#ifndef Stream_h
#define Stream_h

#include "Print.h"


class Stream : public Print {

public:
    void begin(int baud);
    virtual size_t write(uint8_t chr);
    virtual size_t write(const uint8_t *buffer, size_t size);
};

extern Stream Serial;
extern Stream Stdout;

#endif
