/**
 * Mini HTTP Server - demonstrates handling HTTP requests
 *
 * This example handles two endpoints:
 *   /       - Displays "Hello World" as HTML
 *   /text   - Displays some plain text
 *
 * All other requests will display "404 Not Found".
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

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

    } else if (http.havePacket()) {
        // Some other HTTP request, return 404
        http.notFound();

    } else {
        // Some other packet, reply with rejection
        ether.rejectPacket();

    }
}
