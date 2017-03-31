/**
 * HTTP client example
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <EtherSia.h>

/** Ethernet Interface */
EtherSia_ENC28J60 ether;

/** HTTP Client */
HTTPClient http(ether);

void setup() {
    MACAddress macAddress("6e:e7:2f:4c:64:78");

    // Setup serial port
    Serial.begin(57600);
    Serial.println("[EtherSia HTTP Client]");
    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    // Lookup IPv6 address of the server
    http.setRemoteHost(F("time.aelius.com"));

    Serial.println("Ready.");
}

void loop()
{
    // process packets
    ether.receivePacket();

    // Make a request every 10 seconds
    static unsigned long nextRequest = millis();
    if ((long)(millis() - nextConnect) >= 0) {
        Serial.println("Connecting to time server.");
        http.get("/daytime");
        nextRequest = millis() + 10000;
    }

    if (http.havePacket()) {
        char *buf = http.payload();
        buf[http.payloadLength()] = '\0';
        Serial.print("Data: ");
        Serial.println(buf);

        // We must acknowledge that we received the packet
        http.sendAck();
    }
}
