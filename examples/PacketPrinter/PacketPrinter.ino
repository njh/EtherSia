// Packet Printer - prints Ethernet packet headers received to Serial
// 
// Uses a static MAC address, please update with your own.
//
// Get your own Random Locally Administered MAC Address here:
// https://www.hellion.org.uk/cgi-bin/randmac.pl
//

#include <EtherSia.h>

EtherSia ether(8);

void setup() {
    MACAddress macaddr("e2:d7:66:39:6b:5e");

    // Setup serial port for debugging
    Serial.begin(38400);
    Serial.println("[EtherSia PacketPrinter]");

    Serial.print("Our MAC is: ");
    macaddr.println();

    if (ether.begin(&macaddr) == false) {
        Serial.println("Failed to configure Ethernet");
    }
}


void loop() {
    IPv6Packet *packet = ether.receivePacket();

    if (packet) {
        Serial.print("Src MAC: ");
        packet->etherSrc.println();

        Serial.print("Dest MAC: ");
        packet->etherDest.println();

        Serial.print("Src Address: ");
        packet->src.println();

        Serial.print("Dest Address: ");
        packet->dest.println();

        Serial.print("Protocol: ");
        Serial.println(packet->proto, DEC);

        Serial.print("Length: ");
        Serial.println(ntohs(packet->length), DEC);

        Serial.println();
    }
}

