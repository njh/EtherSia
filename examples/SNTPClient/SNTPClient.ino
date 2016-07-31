/**
 * SNTP Client - sends UDP requests to an IPv6 NTP Server and displays the current time
 *
 * SNTP is a simple subnet of the full NTP protocol.
 *
 * See RFC4330 for details of the protocol:
 * - https://tools.ietf.org/html/rfc4330
 *
 * This example demonstrates of the basics of an SNTP client;
 * A real-world client should enforce RFC4330 properly, such as 
 * the randomised time before sending a query at startup.
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
EtherSia ether(10);

/** Define a UDP socket to send packets from */
UDPSocket udp(ether);

/** The UDP port number to send NTP packets to */
const uint8_t NTP_PORT = 123;

/** The size of the NTP request packet */
const uint8_t NTP_PACKET_SIZE = 48;

/** How often to send NTP packets - RFC4330 says this shouldn't be less than 15 seconds */
const uint32_t DEFAULT_POLLING_INTERVAL = 15;


void setup() {
    MACAddress macAddress("76:73:19:ba:b8:19");

    // Setup serial port
    Serial.begin(38400);
    Serial.println("[EtherSia SNTPClient]");
    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Our global address is: ");
    ether.globalAddress().println();

    if (udp.setRemoteAddress("time.ethersia.aelius.com", NTP_PORT)) {
        Serial.print("SNTP Server address: ");
        udp.remoteAddress().println();
    }

    Serial.println("Ready.");
}

// Time of the next NTP request
unsigned long nextRequest = millis();
unsigned long pollingInterval = DEFAULT_POLLING_INTERVAL;

void loop() {
    // process packets
    ether.receivePacket();

    if (udp.havePacket()) {
        Serial.println("Received SNTP reply.");

        Serial.print("address=");
        udp.packetSource().println();
        Serial.print("len=");
        Serial.println(udp.payloadLength(), DEC);

        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        uint8_t *payload = udp.payload();
        unsigned long highWord = word(payload[40], payload[41]);
        unsigned long lowWord = word(payload[42], payload[43]);
        unsigned long seconds = highWord << 16 | lowWord;
        displayTime(seconds);

        // Success, server is working; reset the polling interval to default
        pollingInterval = DEFAULT_POLLING_INTERVAL;
        nextRequest = millis() + (pollingInterval * 1000);
    }

    if ((long)(millis() - nextRequest) >= 0) {
        Serial.println("Sending SNTP request.");

        // Set the NTP packet to all-zeros
        uint8_t ntpPacket[NTP_PACKET_SIZE];
        memset(ntpPacket, 0, NTP_PACKET_SIZE);

        // Set NTP header flags (Leap Indicator=Not Synced, Version=4, Mode=Client)
        ntpPacket[0] = 0xe3;
        udp.send(ntpPacket, NTP_PACKET_SIZE);

        // Exponential back-off; double the polling interval
        // This prevents the server from being overloaded if it is having problems 
        pollingInterval *= 2;

        // Set the time of the next request to: now + polling interval
        nextRequest = millis() + (pollingInterval * 1000);
    }
}


/** Decode a NTP timestamp and print it to Serial */
void displayTime(unsigned long secsSince1900)
{
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");

    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;

    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;

    // print Unix time:
    Serial.println(epoch);

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
    Serial.println();
}
