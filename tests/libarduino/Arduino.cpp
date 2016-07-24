#include "Arduino.h"

uint32_t millis( void ) {return 0;}
uint32_t micros( void ) {return 0;}
void delay(uint32_t /* msec */) {}
void delayMicroseconds(uint32_t /* us */) {}

void pinMode(uint8_t, uint8_t) {}
void digitalWrite(uint8_t, uint8_t) {}
int digitalRead(uint8_t) {return 0;}

long random(long) {return 0;}
long random(long, long) {return 0;}
void randomSeed(unsigned long) {}
