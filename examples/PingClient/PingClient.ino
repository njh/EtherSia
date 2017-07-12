/**
 * Example of sending ICMPv6 Ping packets periodically
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

/** Define Ping Client to send/receive packets */
PingClient ping(ether);

void setup() {
    MACAddress macAddress("0a:2c:8c:ba:66:2d");

    // Setup serial port
    Serial.begin(38400);
    Serial.println("[EtherSia PingClient]");

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    if (ping.setRemoteAddress("ipv6.aelius.com")) {
        Serial.print("Remote address: ");
        ping.remoteAddress().println();
    }

    Serial.println("Ready.");
}

void loop()
{
    // process packets
    ether.receivePacket();

    // Is it time to send a ping?
    static unsigned long nextPing = millis();
    if ((long)(millis() - nextPing) >= 0) {
        // Did the last ping timeout?
        if (ping.sequenceNumber() > 0 && ping.gotReply() == false) {
            Serial.print("Request timeout for icmp_seq ");
            Serial.println(ping.sequenceNumber(), DEC);
        }

        // Send a ping
        ping.send();
        nextPing = millis() + 1000;
    }

    if (ping.havePacket()) {
        // We got a reply
        Serial.print("Pong! icmp_seq=");
        Serial.println(ping.sequenceNumber(), DEC);
    } else {
        // Reject any incoming connections
        ether.rejectPacket();
    }
}

