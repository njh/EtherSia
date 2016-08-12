/**
 * Minimal example - the minimum amount of code to start using EtherSia
 *
 * Doesn't do anything other than print the IP address and respond to pings.
 *
 * Uses a static MAC address, please update with your own.
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <EtherSia.h>

/** Ethernet Interface */
EtherSia_ENC28J60 ether;

void setup() {
    MACAddress macAddress("3e:ad:93:36:7d:9d");

    // Setup serial port for debugging
    Serial.begin(38400);
    Serial.println("[EtherSia Minimal]");

    // Configure the Ethernet interface
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Global address: ");
    ether.globalAddress().println();
}


void loop() {
    ether.receivePacket();

    // Do other stuff here
    // But avoid using delay() or anything that takes a long time

}
