
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
        if (fileno == 1) {
            // Write straight to serial port
            Serial.write(data, len);
        } else if (fileno == 2) {
            // Handle writing to internal EEPROM
            uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;
            for(uint16_t i=0; i<len; i++) {
                uint16_t idx = i+offset;
                if (idx < EEPROM.length()) {
                    EEPROM.update(idx, data[i]);
                }
            }
        } else if (fileno == 3) {
            // Handle writing to external i2c EEPROM
            uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;
            for(uint16_t i=0; i<len; i++) {
                uint16_t idx = i+offset;
                if (idx < AT24C128_LENGTH) {
                    i2c_eeprom_write_byte(AT24C128_ADDRESS, idx, data[i]);
                }
            }
        }
    }

    int16_t readBytes(int8_t fileno, uint16_t block, uint8_t* data)
    {
        if (fileno == 2) {
            // Handle reading from internal EEPROM
            uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;
            uint16_t len = EEPROM.length() - offset;
            if (len > TFTP_BLOCK_SIZE)
                len = TFTP_BLOCK_SIZE;
            for(uint16_t i=0; i<len; i++) {
                data[i] = EEPROM.read(i+offset);
            }
            return len;
        } else if (fileno == 3) {
            // Handle reading from external i2c EEPROM
            uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;
            uint16_t len = AT24C128_LENGTH - offset;
            if (len > TFTP_BLOCK_SIZE)
                len = TFTP_BLOCK_SIZE;
            for(uint16_t i=0; i<len; i++) {
                data[i] = i2c_eeprom_read_byte(AT24C128_ADDRESS, i+offset);
            }
            return len;
        } else {
            // Reading from anything else isn't implemented
            return 0;
        }
    }

private:

    void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
        Wire.beginTransmission(deviceaddress);
        Wire.write((int)(eeaddress >> 8));   // MSB
        Wire.write((int)(eeaddress & 0xFF)); // LSB
        Wire.write(data);
        Wire.endTransmission();
 
        delay(5);
    }
    
    byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
        byte rdata = 0xFF;
        Wire.beginTransmission(deviceaddress);
        Wire.write((int)(eeaddress >> 8));   // MSB
        Wire.write((int)(eeaddress & 0xFF)); // LSB
        Wire.endTransmission();

        Wire.requestFrom(deviceaddress, 1);

        if (Wire.available()) rdata = Wire.read();

        return rdata;
    }

};
