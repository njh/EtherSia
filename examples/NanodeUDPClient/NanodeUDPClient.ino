/**
 * Example of sending UDP packets from a Nanode board
 *
 * Sends a 'Hello Message' every 5 seconds to specificed IPv6 Address.
 * Remember to change the address you send to below in the setRemoteAddress() call.
 *
 * Use the socat command to receive packets sent by this sketch:
 *
 *     socat -u UDP6-LISTEN:1234 STDOUT
 *
 * Requires the NanodeUNIO library:
 * https://github.com/sde1000/NanodeUNIO
 *
 * Nanode Hardware
 * ---------------
 * - CPU: Atmel ATMEGA328P
 * - Ethernet: Microchip ENC28J60
 * - MAC Address: Microchip 11AA02E48 serial EEPROM
 *
 * Nanode Pin Mapping
 * ------------------
 * | Pin | Use                                |
 * |-----|------------------------------------|
 * | D6  | Red LED                            |
 * | D7  | UNI/O bus: MAC Address (11AA02E48) |
 * | D8  | SPI bus: Ethernet Slave Select     |
 * | D11 | SPI bus: Shared MOSI               |
 * | D12 | SPI bus: Shared MISO               |
 * | D13 | SPI bus: Shared Serial Clock       |
 *
 * @file
 */

#include <NanodeUNIO.h>
#include <EtherSia.h>

/** Ethernet Interface (with Chip Select connected to Pin 8) */
EtherSia_ENC28J60 ether(8);

/** Define UDP socket to send packets from */
UDPSocket udp(ether);

void setup() {
    NanodeUNIO unio(NANODE_MAC_DEVICE);
    MACAddress macAddress;
    boolean r;

    // Setup serial port
    Serial.begin(38400);
    Serial.println("[EtherSia NanodeUDPClient]");

    Serial.print("Reading MAC address... ");
    r = unio.read(macAddress, NANODE_MAC_ADDRESS, 6);
    if (r) Serial.println("success");
    else Serial.println("failure");

    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    if (udp.setRemoteAddress("2001:41c8:51:7cf::6", 1234)) {
        Serial.print("Remote address: ");
        udp.remoteAddress().println();
    }

    Serial.println("Ready.");
}

void loop()
{
    // process packets
    ether.receivePacket();

    static unsigned long nextMessage = millis();
    if ((long)(millis() - nextMessage) >= 0) {
        Serial.println("Sending UDP.");
        udp.println("Hello World!");
        udp.send();
        nextMessage = millis() + 5000;
    }

    // Reject any incoming connections
    ether.rejectPacket();
}
