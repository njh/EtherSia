#include <NanodeUNIO.h>
#include <EtherSia.h>

EtherSia ether(8);


void setup() {
    NanodeUNIO unio(NANODE_MAC_DEVICE);
    byte macaddr[6];
    boolean r;

    // Setup serial port
    Serial.begin(38400);
    Serial.println("[EtherSia]");

    Serial.print("Reading MAC address... ");
    r = unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
    if (r) Serial.println("success");
    else Serial.println("failure");

    Serial.print("mymac=");
    ether.print_mac(macaddr);

    // start the Ethernet connection:
    if (ether.begin(macaddr) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    // give the Ethernet shield a second to initialize
    delay(1000);
    Serial.println("Ready.");
}

void loop() {
    // process packets
    ether.loop();
}
