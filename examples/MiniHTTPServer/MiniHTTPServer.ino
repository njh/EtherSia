#include <EtherSia.h>

/** W5500 Interface */
EtherSia_ENC28J60 ether(10);

/** Define HTTP server */
HTTPServer http(ether);


void setup() {
    MACAddress macAddress("9e:b3:19:c7:1b:10");

    // Setup serial port
    Serial.begin(115200);
    Serial.println("[EtherSia MiniHTTPServer]");
    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to configure Ethernet");
    }

    Serial.print("Our link-local address is: ");
    ether.linkLocalAddress().println();
    Serial.print("Our global address is: ");
    ether.globalAddress().println();

    Serial.println("Ready.");
}

void loop() {
    ether.receivePacket();

    if (http.isGet("/")) {
        Serial.print("Received HTTP request from: ");
        http.packetSource().println();
        
        http.println(F("HTTP/1.0 200 OK"));
        http.println(F("Content-Type: text/html"));
        http.println();
        http.println(F("<h1>Hello World</h1>"));
        http.sendReply();
    } else {
        http.notFound();
    }
}
