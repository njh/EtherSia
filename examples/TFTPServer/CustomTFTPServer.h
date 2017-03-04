
#include <EtherSia.h>


class CustomTFTPServer: public TFTPServer {

public:
    CustomTFTPServer(EtherSia &ether) : TFTPServer(ether) {};

    int8_t openFile(const char* filename)
    {
        if (strcmp(filename, "serial") == 0) {
            // Serial port is File Number: 1
            return 1;
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
        }
    }

    int16_t readBytes(int8_t fileno, uint16_t block, uint8_t* data)
    {
        // Reading isn't implemented
        return 0;
    }

};
