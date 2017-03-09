
#include <EtherSia.h>
#include <EEPROM.h>
#include <Wire.h>



/**
 * Custom TFTP server class
 *
 * Supports the following filenames:
 *   serial: serial port (write only)
 *   eeprom: the built-in EEPROM (read/write)
 *   i2c: an external AT24C128 EEPROM connected to i2c
 *
 */
class CustomTFTPServer: public TFTPServer {

    const uint16_t AT24C128_LENGTH = 16384;
    const uint8_t AT24C128_ADDRESS = 0x50;
    const uint8_t AT24C128_BUF_SIZE = 32;   // This is the size of the Arduino buffer

public:
    /**
     * Construct a new custom TFTP server
     *
     * @param ether The Ethernet interface to attach the TFTP server to
     */
    CustomTFTPServer(EtherSia &ether) : TFTPServer(ether) {};

    int8_t openFile(const char* filename)
    {
        if (strcmp(filename, "serial") == 0) {
            // Serial port is File Number: 1
            return 1;
        } else if (strcmp(filename, "eeprom") == 0) {
            // Internal EEPROM is File Number: 2
            return 2;
        } else if (strcmp(filename, "i2c") == 0) {
            // External i2c EEPROM is File Number: 3
            return 3;
        } else {
            // File Not Found
            return -1;
        }
    }

    void writeBytes(int8_t fileno, uint16_t block, const uint8_t* data, uint16_t len)
    {
        uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;

        if (fileno == 1) {
            // Write straight to serial port
            Serial.write(data, len);
        } else if (fileno == 2) {
            // Handle writing to internal EEPROM
            for(uint16_t i=0; i<len; i++) {
                uint16_t idx = i+offset;
                if (idx < EEPROM.length()) {
                    EEPROM.update(idx, data[i]);
                }
            }
        } else if (fileno == 3) {
            // Handle writing to external i2c EEPROM
            writeToI2c(offset, data, len);
        }
    }

    int16_t readBytes(int8_t fileno, uint16_t block, uint8_t* data)
    {
        uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;

        if (fileno == 2) {
            // Handle reading from internal EEPROM
            uint16_t len = EEPROM.length() - offset;
            if (len > TFTP_BLOCK_SIZE)
                len = TFTP_BLOCK_SIZE;
            for(uint16_t i=0; i<len; i++) {
                data[i] = EEPROM.read(i+offset);
            }
            return len;
        } else if (fileno == 3) {
            // Handle reading from external i2c EEPROM
            return readFromI2c(offset, data);
        } else {
            // Reading from anything else isn't implemented
            return 0;
        }
    }

private:

    void writeToI2c(uint16_t offset, const uint8_t* data, uint16_t len)
    {
        // Handle writing to external i2c EEPROM
        for(uint16_t i=0; i<len;) {
            uint16_t eeaddress = i+offset;

            if (eeaddress >= AT24C128_LENGTH)
                return;

            Wire.beginTransmission(AT24C128_ADDRESS);
            Wire.write((int)(eeaddress >> 8));   // MSB
            Wire.write((int)(eeaddress & 0xFF)); // LSB

            // WARNING: this page write only works because the TFTP block size (512)
            // is divisible by the EEPROM page size (64 bytes)
            // But it must also fit within the Arduino 32 byte Wire buffer
            uint16_t end = i + (AT24C128_BUF_SIZE/2);
            while(i<end && i<len) {
                Wire.write(data[i++]);
            }
            Wire.endTransmission();

            // Give time to allow the EEPROM to write the data
            delay(5);
        }
    }

    int16_t readFromI2c(uint16_t offset, uint8_t* data)
    {
        uint16_t len = AT24C128_LENGTH - offset;
        if (len > TFTP_BLOCK_SIZE)
            len = TFTP_BLOCK_SIZE;

        for(uint16_t i=0; i<len;) {
            int eeaddress = i+offset;
            Wire.beginTransmission(AT24C128_ADDRESS);
            Wire.write((int)(eeaddress >> 8));   // MSB
            Wire.write((int)(eeaddress & 0xFF)); // LSB
            Wire.endTransmission();

            Wire.requestFrom(AT24C128_ADDRESS, AT24C128_BUF_SIZE);

            uint16_t end = i + AT24C128_BUF_SIZE;
            while(i<end) {
                if (!Wire.available()) {
                    // No more data available - End of File
                    Serial.println("No more data available");
                    return i;
                }
                data[i++] = Wire.read();
            }
        }
        return len;
    }

};
