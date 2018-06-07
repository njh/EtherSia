#include "SPI.h"

void SPIClass::begin() {}

uint8_t SPIClass::transfer(uint8_t val) { return val; }

void SPIClass::end() {}

void SPIClass::setBitOrder(uint8_t) {}
void SPIClass::setDataMode(uint8_t) {}
void SPIClass::setClockDivider(uint8_t) {}
void SPIClass::beginTransaction(SPISettings /*settings*/) {}
void SPIClass::endTransaction() {}
