/**
 * Example of receiving UDP packets from a Nanode board
 *
 * Allows you to control the Nanode LED by sending "on" and "off" UDP packets
 *
 * You can use the 'echo' and 'socat' commands to send UDP packets using:
 *
 *     echo -n "on" | socat -u STDIN UDP-DATAGRAM:<ip address>:1234
 *     echo -n "off" | socat -u STDIN UDP-DATAGRAM:<ip address>:1234
 *
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

/** The pin number the Red LED on Nanode is connected to */
const int NANODE_LED_PIN = 6;

/** ENC28J60 Interface (with Chip Select connected to Pin 8) */
EtherSia_ENC28J60 ether(8);

/** Define UDP socket and port number to listen on */
UDPSocket udp(ether, 1234);


void setup() {
    NanodeUNIO unio(NANODE_MAC_DEVICE);
    MACAddress macAddress;
    boolean r;

    // Setup serial port
    Serial.begin(38400);
    Serial.println("[EtherSia NanodeUDPServer]");

    Serial.print("Reading MAC address... ");
    r = unio.read(macAddress, NANODE_MAC_ADDRESS, 6);
    if (r) Serial.println("success");
    else Serial.println("failure");

    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Our link-local address is: ");
    ether.linkLocalAddress().println();
    Serial.print("Our global address is: ");
    ether.globalAddress().println();

    // Turn off the Red LED (it is connected to +5v)
    pinMode(NANODE_LED_PIN, OUTPUT);
    digitalWrite(NANODE_LED_PIN, HIGH);

    Serial.println("Ready.");
}

void loop() {
    ether.receivePacket();

    if (udp.havePacket()) {
        Serial.print("Received UDP from: ");
        udp.packetSource().println();

        Serial.print("Packet length: ");
        Serial.println(udp.payloadLength(), DEC);

        if (udp.payloadEquals("on")) {
            Serial.println("** LED On **");
            digitalWrite(NANODE_LED_PIN, LOW);
            udp.sendReply("ok");
        } else if (udp.payloadEquals("off")) {
            Serial.println("** LED Off **");
            digitalWrite(NANODE_LED_PIN, HIGH);
            udp.sendReply("ok");
        } else {
            Serial.println("Invalid Message");
            udp.sendReply("err");
        }
    } else {
        // Send back a ICMPv6 Destination Unreachable response
        // to any other connection attempts
        ether.rejectPacket();
    }
}
