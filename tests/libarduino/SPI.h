#include <stdint.h>

#ifndef ARDUINO_SPI
#define ARDUINO_SPI 1

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C
#define SPI_CLOCK_MASK 0x03
#define SPI_2XCLOCK_MASK 0x01

static const uint8_t SS   = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK  = 13;

class SPISettings {
public:
  SPISettings(uint32_t /*clock*/, uint8_t /*bitOrder*/, uint8_t /*dataMode*/) {
  }
};

class SPIClass {
public:
  static void begin();

  static uint8_t transfer(uint8_t val);

  static void end();

  static void beginTransaction(SPISettings settings);
  static void endTransaction();

  static void setBitOrder(uint8_t bitOrder);
  static void setDataMode(uint8_t dataMode);
  static void setClockDivider(uint8_t clockDiv);
};

extern SPIClass SPI;

#endif
