/**
 * EmonCMS TCP Client example
 * tested with arduino Uno/Nano and emonCMS sofware from SD release "emonSD-07Nov16"
 * http://files.openenergymonitor.org/emonSD-07Nov16.zip
 * https://github.com/openenergymonitor/emonpi/wiki/emonSD-pre-built-SD-card-Download-&-Change-Log
 * Alexandre CUER 14/03/2017
 */

#include <EtherSia.h>

#define DEBUG (1)

static unsigned long lastConnect = 0;
bool availableData = false;
boolean dataSent = false;

char emonapikey[] = "your32bitskey";//HOME
uint16_t emonport=80;
const char *emonip="fe80::ba27:ebff:fede:64b3";//HOME
//const char *emonip="fe80::ba27:ebff:fe84:c0a1";//OFFICE
//you can find the ipv6 address of your local emoncms server via the shell command ifconfig eth0


/** Ethernet Interface */
EtherSia_ENC28J60 ether;

/** Define TCP socket to send messages from */
TCPClient tcp(ether);


void setup() {

    // Setup serial port
    Serial.begin(115200);
    
    MACAddress macAddress("6e:e7:2f:4c:64:78");
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
    Serial.println("Ready.");
}

void loop()
{
    
    ether.receivePacket();
    availableData=tcp.havePacket();

    /*
    if(tcp.connected()) {
        if(DEBUG) Serial.println("successfull connection");
      }
    */  
    
    if(tcp.connected() && availableData && dataSent) {
        // READING DATA
        uint8_t *buf = tcp.payload();
        buf[tcp.payloadLength()] = '\0';
        Serial.print("Srv answer: ");
        Serial.println((char*)(&(buf[0])));
        tcp.disconnect();
        dataSent=false;
        availableData=false;
      }
    
    //in case millis has returned to 0 between two consecutive connections 
    //a return to zero of millis() which can occur after 50 days 
    if(millis()-lastConnect < 0)lastConnect =0;
    
    if (!tcp.connected() && (long)(millis() - lastConnect) >= 10000) { 
        if(DEBUG) {
          Serial.println();Serial.println();
          Serial.println("...Connecting emonserver.");
        }
        Serial.println(lastConnect);
        tcp.connect();
        lastConnect=millis();
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
    }
    

}
