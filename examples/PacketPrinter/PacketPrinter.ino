/**
 * Packet Printer - prints Ethernet and IPv6 packet headers received to Serial
 *
 * Uses a static MAC address, please update with your own.
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <EtherSia.h>

/** Ethernet Interface (with Chip Select connected to Pin 10) */
EtherSia ether(10);

void setup() {
    MACAddress macAddress("e2:d7:66:39:6b:5e");

    // Setup serial port for debugging
    Serial.begin(38400);
    Serial.println("[EtherSia PacketPrinter]");

    Serial.print("Our MAC is: ");
    macAddress.println();

    if (ether.begin(&macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }
}


void loop() {
    IPv6Packet *packet = ether.receivePacket();

    if (packet) {
        Serial.print("Source MAC: ");
        packet->etherSource.println();

        Serial.print("Destination MAC: ");
        packet->etherDestination.println();

        Serial.print("Source Address: ");
        packet->source.println();

        Serial.print("Destination Address: ");
        packet->destination.println();

        Serial.print("Protocol: ");
        Serial.println(packet->protocol, DEC);

        Serial.print("Length: ");
        Serial.println(ntohs(packet->length), DEC);

        Serial.println();
    }
}

