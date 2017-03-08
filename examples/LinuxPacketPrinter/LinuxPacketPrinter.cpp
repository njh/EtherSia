/**
 * Linux Packet Printer - prints Ethernet and IPv6 packet headers on Linux (not Arduino)
 *
 * This example demonstrates that it is possible to run EtherSia on Linux, using
 * a raw socket to send and receive Ethernet frames.
 *
 * Type `make` in the LinuxPacketPrinter directory to build this example.
 * Then type `sudo ./LinuxPacketPrinter` to run the example.
 *
 * It must be run as root, so that the program has permission to send and
 * receive Ethernet frames.
 *
 * Uses a static MAC address, please update with your own.
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <EtherSia.h>

/** Ethernet Interface to use */
EtherSia_LinuxSocket ether("eth0");


/**
 * Main function in Linux Packet Printer example
 *
 * @return 0 if successful
 */
int main()
{
    MACAddress macAddress("5e:73:f9:8a:cf:ba");

    Serial.println("[EtherSia LinuxPacketPrinter]");
    Serial.print("Our MAC is: ");
    macAddress.println();

    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Link Local Address: ");
    ether.linkLocalAddress().println();
    Serial.print("Global Address: ");
    ether.globalAddress().println();

    while (true) {
        if (ether.receivePacket()) {
            IPv6Packet& packet = ether.packet();

            Serial.print("Source MAC: ");
            packet.etherSource().println();

            Serial.print("Destination MAC: ");
            packet.etherDestination().println();

            Serial.print("Source Address: ");
            packet.source().println();

            Serial.print("Destination Address: ");
            packet.destination().println();

            Serial.print("Protocol: ");
            Serial.println(packet.protocol(), DEC);

            Serial.print("Length: ");
            Serial.println(packet.payloadLength(), DEC);

            Serial.println();
        }
    }

    return 0;
}
