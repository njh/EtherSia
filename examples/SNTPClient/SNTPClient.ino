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
#include "ntp.h"

/** Wiznet W5100 ethernet interface (with Chip Select connected to Pin 10) */
EtherSia_W5100 ether(10);

/** Define a UDP socket to send packets from */
UDPSocket udp(ether);

/** How often to send NTP packets - RFC4330 says this shouldn't be less than 15 seconds */
const uint32_t DEFAULT_POLLING_INTERVAL = 15;


void setup() {
    MACAddress macAddress("76:73:19:ba:b8:19");

    // Setup serial port
    Serial.begin(115200);
    Serial.println("[EtherSia SNTPClient]");
    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Our global address is: ");
    ether.globalAddress().println();

    Serial.println("Ready.");
}

/** Set to true when a DNS lookup needs to be performed */
boolean needNewServer = true;

/** The time that the next NTP request should be sent */
unsigned long nextRequest = millis();

/** How often (in seconds) that NTP requests should be sent */
unsigned long pollingInterval = DEFAULT_POLLING_INTERVAL;


void loop() {
    // process packets
    ether.receivePacket();

    // Do we need to lookup the IP address of a new server?
    if (needNewServer) {
        if (udp.setRemoteAddress("time.ethersia.aelius.com", NTP_PORT)) {
            Serial.print("SNTP Server address: ");
            udp.remoteAddress().println();
            needNewServer = false;
        } else {
            // Something went wrong, sleep for a minute, then try again
            Serial.println("Error looking up remote host");
            delay(60000);
            return;
        }
    }

    if (udp.havePacket()) {
        Serial.println("Received SNTP reply.");

        Serial.print("address=");
        udp.packetSource().println();
        Serial.print("len=");
        Serial.println(udp.payloadLength(), DEC);

        // Convert the payload into the NTP structure above
        ntpType *ntpPacket = (ntpType*)udp.payload();

        Serial.print("Server stratum: ");
        Serial.println(ntpPacket->stratum, DEC);

        // If stratum value is 0, then it is a "Kiss-o'-Death" packet
        if (ntpPacket->stratum == 0) {
            // The server wants us to go away
            Serial.println("Kiss-o'-Death!");
            needNewServer = true;
            return;
        } else {
            // Extract the transmit timestamp from the packet
            // this is NTP time (seconds since Jan 1 1900)
            // The ntohl() function converts from network byte-order to native byte-order
            displayTime(
                ntohl(ntpPacket->transmitTimestampSeconds)
            );

            // Success, server is working; reset the polling interval to default
            pollingInterval = DEFAULT_POLLING_INTERVAL;
            nextRequest = millis() + (pollingInterval * 1000);
        }
    } else {
        // Reject any other incoming packets
        ether.rejectPacket();
    }

    if ((long)(millis() - nextRequest) >= 0) {
        Serial.println("Sending SNTP request.");

        // Set the NTP packet to all-zeros
        ntpType ntpPacket;
        memset(&ntpPacket, 0, sizeof(ntpPacket));

        // Set NTP header flags (Leap Indicator=Not Synced, Version=4, Mode=Client)
        ntpPacket.flags = 0xe3;
        udp.send(&ntpPacket, sizeof(ntpPacket));

        // Exponential back-off; double the polling interval
        // This prevents the server from being overloaded if it is having problems
        pollingInterval *= 2;

        // Set the time of the next request to: now + polling interval
        nextRequest = millis() + (pollingInterval * 1000);
    }
}


/**
 * Decode a NTP timestamp and print it to Serial
 *
 * @param secs The number of seconds since the year 1900
 */
void displayTime(unsigned long secs)
{
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secs);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");

    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;

    // subtract seventy years:
    unsigned long epoch = secs - seventyYears;

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
