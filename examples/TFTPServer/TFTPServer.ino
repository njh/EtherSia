/**
 * Minimal TFTP Server example (RFC 1350)
 *
 * This sketch allows you to:
 *   - write a file to the serial port (using the 'serial' filename)
 *   - read/write the internal EEPROM (using the 'eeprom' filename)
 *   - read/write an external AT24C128 EEPROM (using the 'i2c' filename)
 *
 * On your computer open a TFTP tool and connect to the Arduino.
 * Switch to 'binary' mode and then upload a local filename to the 'serial'
 * remote filename.
 *
 *    tftp 2a00:1098:8:68:6084:98ff:fe22:092c
 *    tftp> binary
 *    tftp> put README.md serial
 *
 *
 * This example uses a static MAC address, please update with your own.
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <EtherSia.h>
#include "CustomTFTPServer.h"


/** Ethernet Interface (with Chip Select connected to Pin 10) */
EtherSia_ENC28J60 ether(10);

/** Instance of our custom TFTP server */
CustomTFTPServer tftp(ether);

void setup() {
    MACAddress macAddress("62:84:98:22:09:2c");

    // Setup serial port
    Serial.begin(57600);
    Serial.println("[EtherSia TFTPServer]");

    // Setup i2c
    Wire.begin();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Our link-local address is: ");
    ether.linkLocalAddress().println();
    Serial.print("Our global address is: ");
    ether.globalAddress().println();

    Serial.println("Ready.");
}


void loop() {
    ether.receivePacket();

    if (!tftp.handleRequest()) {
        // If TFTP didn't handle the request, reject the packet
        ether.rejectPacket();
    }
}
