/**
 * Example of sending Syslog messages from EtherSia
 *
 * You can print log lines to the syslog object.
 * Log lines will be sent upon writing a newline character (eg using println).
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
EtherSia_W5100 ether;

/** Define the syslog UDP socket object */
Syslog syslog(ether);


void setup() {
    MACAddress macAddress("0a:2c:8c:ba:66:2d");

    // Start Ethernet
    ether.begin(macAddress);

    // Set the address of the syslog server
    syslog.setRemoteAddress("logger.example.com");
}


void loop() {
    ether.receivePacket();

    static unsigned long nextMessage = millis();
    if ((long)(millis() - nextMessage) >= 0) {
        syslog.print("Seconds since boot=");
        syslog.println(millis() / 1000, DEC);
        nextMessage = millis() + 5000;
    }

    // Reject any incoming connections
    ether.rejectPacket();
}
