#include <EtherSia.h>

/** W5100 Ethernet Interface */
EtherSia_W5100 ether;

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

    if (http.isGet(F("/"))) {
        http.printHeaders(http.typeHtml);
        http.println(F("<h1>Hello World</h1>"));
        http.sendReply();

    } else if (http.isGet(F("/text"))) {
        http.printHeaders(http.typePlain);
        http.println(F("This is some plain text"));
        http.sendReply();

    } else {
        http.notFound();
    }
}
