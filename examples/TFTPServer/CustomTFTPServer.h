
#include <EtherSia.h>
#include <EEPROM.h>



/**
 * Custom TFTP server class
 *
 * Supports the following filenames:
 *   serial: serial port (write only)
 *   eeprom: the built-in EEPROM (read/write)
 *
 */
class CustomTFTPServer: public TFTPServer {

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
            // Handle writing to EEPROM
            uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;
            for(uint16_t i=0; i<len; i++) {
                uint16_t idx = i+offset;
                if (idx < EEPROM.length()) {
                    EEPROM.update(idx, data[i]);
                }
            }
        }
    }

    int16_t readBytes(int8_t fileno, uint16_t block, uint8_t* data)
    {
        if (fileno == 2) {
            // Handle reading from EEPROM
            uint16_t offset = (block-1) * TFTP_BLOCK_SIZE;
            uint16_t len = EEPROM.length() - offset;
            if (len > TFTP_BLOCK_SIZE)
                len = TFTP_BLOCK_SIZE;
            for(uint16_t i=0; i<len; i++) {
                data[i] = EEPROM.read(i+offset);
            }
            return len;
        } else {
            // Reading from anything else isn't implemented
            return 0;
        }
    }

};
