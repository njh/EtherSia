#ifndef Stream_h
#define Stream_h

#include "Print.h"


class Stream : public Print {

public:
    void begin(int baud);
};

extern Stream Serial;

#endif
