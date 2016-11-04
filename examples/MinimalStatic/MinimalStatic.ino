/**
 * Minimal static example - example of disabling stateless autoconfiguration
 *
 * Doesn't do anything other than configure a static IP address and respond to pings.
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
    MACAddress macAddress("e6:a6:57:21:ec:d1");

    // Setup serial port for debugging
    Serial.begin(57600);
    Serial.println("[EtherSia MinimalStatic]");

    ether.disableAutoconfiguration();

    // Start the Ethernet interface
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    // Configure a static global address and router addresses
    ether.setGlobalAddress("2001:1234::5000");
    if (ether.setRouter("fe80::f4c0:4ff:fefb:4186") == false) {
        Serial.println("Failed to configure router address");
    }

    Serial.println("Ready");
}


void loop() {
    ether.receivePacket();

    // Do other stuff here
    // But avoid using delay() or anything that takes a long time

}
