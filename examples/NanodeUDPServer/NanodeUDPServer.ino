#include <NanodeUNIO.h>
#include <EtherSia.h>


const int NANODE_LED_PIN = 6;
const int UDP_PORT = 1234;


EtherSia ether(8);
UDPSocket udp(&ether, UDP_PORT);


void setup() {
    NanodeUNIO unio(NANODE_MAC_DEVICE);
    MACAddress macaddr;
    boolean r;

    // Setup serial port
    Serial.begin(38400);
    Serial.println("[EtherSia]");

    Serial.print("Reading MAC address... ");
    r = unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
    if (r) Serial.println("success");
    else Serial.println("failure");

    macaddr.println();

    // Start Ethernet
    if (ether.begin(&macaddr) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Our link-local address is: ");
    ether.getLinkLocalAddress()->println();
    Serial.print("Our global address is: ");
    ether.getGlobalAddress()->println();

    // Turn off the Red LED (it is connected to +5v)
    pinMode(NANODE_LED_PIN, OUTPUT);
    digitalWrite(NANODE_LED_PIN, HIGH);

    Serial.println("Ready.");
}

void loop() {
    ether.receivePacket();

    if (udp.havePacket()) {
        Serial.print("Received UDP from: ");
        udp.packetSource()->println();

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
    }
}
