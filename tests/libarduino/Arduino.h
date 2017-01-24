#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

extern "C" {
    typedef uint16_t word;
    typedef uint8_t byte ;
    typedef bool boolean ;

    void* malloc(size_t size);
    void free(void *ptr);
}

/* sketch */
extern void setup( void ) ;
extern void loop( void ) ;

uint32_t millis( void );
uint32_t micros( void );
void delay(uint32_t msec);
void delayMicroseconds(uint32_t us);

void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
int digitalRead(uint8_t);

long random();
long random(long);
long random(long, long);
void randomSeed(unsigned long);

boolean isWhitespace(int c);


#include "Progmem.h"

#include "Print.h"
#include "Stream.h"
#include "Buffer.h"

#endif // Arduino_h
