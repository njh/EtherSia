/**
 * EmonCMS TCP Client example
 * tested with arduino Uno/Nano and emonCMS sofware from SD release "emonSD-07Nov16"
 * http://files.openenergymonitor.org/emonSD-07Nov16.zip
 * https://github.com/openenergymonitor/emonpi/wiki/emonSD-pre-built-SD-card-Download-&-Change-Log
 * Alexandre CUER 25/04/2017
 */

#include <EtherSia.h>

#define DEBUG (1)

static unsigned long lastConnect = 0;
bool availableData = false;
boolean dataSent = false;

char emonapikey[] = "your32bitskey";
uint16_t emonport=80;
//const char *emonip="fe80::ba27:ebff:fede:64b3";//HOME
const char *emonip="fe80::ba27:ebff:fe84:c0a1";//OFFICE
//you can find the ipv6 address of your local emoncms server via the shell command ifconfig eth0


/** Ethernet Interface */
//it should work with ENC28J60
EtherSia_W5100 ether;

/** Define TCP socket to send messages from */
TCPClient tcp(ether);


void setup() {

    // Setup serial port
    Serial.begin(115200);
    
    MACAddress macAddress("6e:e7:2f:4c:64:79");
    Serial.println("[EtherSia client]");
    macAddress.println();

    // Start Ethernet on the device - global and local link
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to get a global address");
    }
    Serial.print("Our link-local address is: ");
    ether.linkLocalAddress().println();

    // Remote server configuration
    if (tcp.setRemoteAddress(emonip, emonport)) {
        Serial.print("emoncms server IPv6 address : ");
        tcp.remoteAddress().println();
    }
    Serial.print("TCPClient Ready.in V");Serial.println(tcp.version());
}

void loop()
{
    
    ether.receivePacket();
    availableData=tcp.havePacket();

    
    //in case millis has returned to 0 between two consecutive connections 
    //a return to zero of millis() which can occur after 50 days 
    if(millis()-lastConnect < 0)lastConnect =0;
    
    if (!tcp.connected() && (long)(millis() - lastConnect) >= 30000) { 
        if(DEBUG) {
          Serial.println();Serial.println();
          Serial.println("...Connecting emonserver.");
        }
        Serial.println(lastConnect);
        tcp.connect();
        lastConnect=millis();
        dataSent=false;
    }
    
    if (tcp.connected() && !dataSent) {
        // 1 - DATA PREPARATION
        tcp.print("GET /emoncms/input/post.json?node=0&json={power:");
        uint32_t a = millis();
        tcp.print(a);
        tcp.print("}&apikey=");
        //if wish of an example without any data posting and only a time request, comment previous 3 lines and uncomment next one
        //tcp.print("GET /emoncms/time/local.json?&apikey=");
        tcp.println(emonapikey);

        // 2 - DATA POST
        tcp.send();
        if(DEBUG) {
          Serial.println(a);
          Serial.print("sending ");
          Serial.print(tcp.payloadLength());
          Serial.println(" bytes of data...");
        }
        dataSent=true;
        //if wish of an active close use the disconnect() method
        //tcp.disconnect();
    }
}
