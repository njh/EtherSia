/**
 * DayTime TCP Client example
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <EtherSia.h>

/** Ethernet Interface */
EtherSia_ENC28J60 ether;

/** Define TCP socket to send messages from */
TCPClient tcp(ether);

void setup() {
    MACAddress macAddress("6e:e7:2f:4c:64:78");

    // Setup serial port
    Serial.begin(57600);
    Serial.println("[EtherSia TimeClient]");
    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    if (tcp.setRemoteAddress("time.ethersia.aelius.com", 13)) {
        Serial.print("Remote address: ");
        tcp.remoteAddress().println();
    }

    Serial.println("Ready.");
}

void loop()
{
    // process packets
    ether.receivePacket();

    // Request the time every 10 seconds
    static unsigned long nextConnect = millis();
    if ((long)(millis() - nextConnect) >= 0) {
        Serial.println("Connecting to time server.");
        tcp.connect();
        nextConnect = millis() + 10000;
    }

    if (tcp.havePacket()) {
        char *buf = tcp.payload();
        buf[tcp.payloadLength()] = '\0';
        Serial.print("Data length: ");
        Serial.println(tcp.payloadLength(), DEC);
        Serial.print("Data: ");
        Serial.println(buf);

        // We must acknowledge that we received the packet 
        tcp.sendAck();
    }
}
