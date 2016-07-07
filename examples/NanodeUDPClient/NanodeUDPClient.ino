// Remember to change the address you send to below in the setDestination() call
//
// Use the socat command to receive packets sent by this sketch:
//
// socat -u UDP6-LISTEN:1234 STDOUT
//

#include <NanodeUNIO.h>
#include <EtherSia.h>

EtherSia ether(8);
UDPSocket udp(&ether);

void setup() {
    NanodeUNIO unio(NANODE_MAC_DEVICE);
    MACAddress macaddr;
    boolean r;

    // Setup serial port
    Serial.begin(38400);
    Serial.println("[NanodeUDPClient]");

    Serial.print("Reading MAC address... ");
    r = unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
    if (r) Serial.println("success");
    else Serial.println("failure");

    macaddr.println();

    // Start Ethernet
    if (ether.begin(&macaddr) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    if (udp.setDestination("2001:41c8:51:7cf::6", 1234)) {
        Serial.print("Destination address: ");
        udp.getDestinationAddress()->println();
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
        udp.send("Hello World!\n");
        nextMessage = millis() + 5000;
    }
}
