/**
 * Minimal TFTP Server example (RFC 1350)
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

/** Ethernet Interface (with Chip Select connected to Pin 10) */
EtherSia_ENC28J60 ether(10);

TFTPServer tftp(ether);


void setup() {
    MACAddress macAddress("62:84:98:22:09:2c");

    // Setup serial port
    Serial.begin(57600);
    Serial.println("[EtherSia TFTPServer]");

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

    tftp.handleRequest();
}
