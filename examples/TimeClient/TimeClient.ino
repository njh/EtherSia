/**
 * Example of a basic TCP client, that connects to a TCP daytime server (RFC 867)
 *
 * A daytime server listens for TCP connections on port 13.  Once a
 * connection is established the current date and time is sent out the
 * connection as an ASCII character string, then the connection is closed.
 *
 * The the daytime protocol is typically implemented in inetd.
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

/** Define TCP socket to send messages from */
TCPClient client(ether);

void setup() {
    MACAddress macAddress("2e:70:f1:3a:e7:37");

    // Setup serial port
    Serial.begin(57600);
    Serial.println("[EtherSia TimeClient]");

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Our link-local address is: ");
    ether.linkLocalAddress().println();
    Serial.print("Our global address is: ");
    ether.globalAddress().println();

    if (client.setRemoteAddress("fdd7:338b:5691::1", 13)) {
        Serial.print("Remote address: ");
        client.remoteAddress().println();
    } else {
        Serial.println("Failed to get remote address.");
    }

    Serial.println("Ready.");
}

void loop()
{
    // process packets
    ether.receivePacket();

    if (client.havePacket()) {
        Serial.print("Have packet: ");
        Serial.write(client.payload(), client.payloadLength());
        Serial.println();

        // Acknowledge the packet and close the connection
        client.disconnect();
    }

    static unsigned long nextConnect = millis();
    if ((long)(millis() - nextConnect) >= 0) {
        Serial.println("Connecting to Daytime server.");
        client.connect();
        // Get the time again in 60 seconds
        nextConnect = millis() + 60000;
    }
}
