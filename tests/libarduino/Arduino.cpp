#include "Arduino.h"

uint32_t millis( void ) {return 0;}
uint32_t micros( void ) {return 100;}
void delay(uint32_t /* msec */) {}
void delayMicroseconds(uint32_t /* us */) {}

void pinMode(uint8_t, uint8_t) {}
void digitalWrite(uint8_t, uint8_t) {}
int digitalRead(uint8_t) {return 0;}

long random() {return 0x55555555;}
long random(long max) {return max/2;}
long random(long min, long max) {return ((max-min)/2)+min;}
void randomSeed(unsigned long) {}

boolean isWhitespace(int c)
{
    return (isblank (c) == 0 ? false : true);
}
