/**
 * MQTT-SN Publish - Example of publishing messages over MQTT-SN
 *
 *
 * Uses a static MAC address, please update with your own.
 *
 * Get your own Random Locally Administered MAC Address here:
 * https://www.hellion.org.uk/cgi-bin/randmac.pl
 *
 * @file
 */

#include <EtherSia.h>

/** ENC28J60 Ethernet Interface */
EtherSia_ENC28J60 ether(10);

/** Define MQTT-SN client */
MQTTSNClient client(ether);



/** Called once at the start */
void setup()
{
    MACAddress macAddress("72:38:91:16:a5:29");

    Serial.begin(115200);
    Serial.println(F("[EtherSia MQTT-SN Publish]"));
    macAddress.println();

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println(F("Failed to configure Ethernet"));
    }

    Serial.print(F("Our address is: "));
    ether.globalAddress().println();

    // Set the address of the MQTT-SN server
    if (client.setRemoteAddress("fdd7:338b:5691::1") == false) {
        Serial.println(F("Failed to set remote address"));
    }

    Serial.println(F("Ready."));


    client.connect();
}

/** the loop function runs over and over again forever */
void loop()
{
    ether.receivePacket();
  
    if (client.checkConnected() == false) {
        Serial.println(F("Not connected"));
    } else {

        // We are connected to the MQTT-SN server: send a message every 5 seconds
        static unsigned long nextMessage = millis();
        if ((long)(millis() - nextMessage) >= 0) {
            Serial.println(F("Publishing to MQTT-SN"));
            client.publish("SN", "Hello World", MQTT_SN_QOS_0);
            nextMessage = millis() + 5000;
        }

    }
}
