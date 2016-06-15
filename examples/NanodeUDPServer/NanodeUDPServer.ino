#include <NanodeUNIO.h>
#include <EtherSia.h>

EtherSia ether(8);

const int NANODE_LED_PIN = 6;


void udp_callback(uint16_t port, const uint8_t *address, char *data, uint16_t len) {
    boolean success = false;

    Serial.println("Received UDP packet from: ");
    ether.print_address(address);

    if (strncmp("on", data, len) == 0) {
        Serial.println("** LED On **");
        digitalWrite(NANODE_LED_PIN, LOW);
        success = true;
    } else if (strncmp("off", data, len) == 0) {
        Serial.println("** LED Off **");
        digitalWrite(NANODE_LED_PIN, HIGH);
        success = true;
    }

    if (success) {
        ether.udp_send_reply("ok");
    } else {
        ether.udp_send_reply("err");
    } 
}


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
    if (ether.begin(macaddr) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    // Turn off the Red LED (it is connected to +5v)
    pinMode(NANODE_LED_PIN, OUTPUT);
    digitalWrite(NANODE_LED_PIN, HIGH);

    ether.udp_listen(&udp_callback, 1234);

    Serial.println("Ready.");
}

void loop() {
    // process packets
    ether.loop();
}
