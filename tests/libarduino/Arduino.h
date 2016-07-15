#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stdlib.h>

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2


extern "C" {
    typedef uint8_t byte ;
    typedef bool boolean ;

    /* sketch */
    extern void setup( void ) ;
    extern void loop( void ) ;

    uint32_t millis( void );
    void delay(uint32_t msec);
    void delayMicroseconds(uint32_t us);

    void pinMode(uint8_t, uint8_t);
    void digitalWrite(uint8_t, uint8_t);
    int digitalRead(uint8_t);
}

#define PROGMEM
#define F(x) x
#define pgm_read_byte_near(x) *(x)



#include "Print.h"
#include "Stream.h"

#endif // Arduino_h
