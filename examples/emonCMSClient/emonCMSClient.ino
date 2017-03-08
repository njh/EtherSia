/**
 * EmonCMS TCP Client example
 * tested with arduino Uno/Nano and emonCMS sofware from SD release "emonSD-07Nov16"
 * http://files.openenergymonitor.org/emonSD-07Nov16.zip
 * https://github.com/openenergymonitor/emonpi/wiki/emonSD-pre-built-SD-card-Download-&-Change-Log
 * Alexandre CUER 04/03/2017
 */
static unsigned long lastConnect = 0;

char emonapikey[] = "your_32_bits_API_here";
uint16_t emonport=80;
const char *emonip="fe80::ba27:ebff:fede:64b3";
//you can find the ipv6 address of your local emoncms server via the shell command ifconfig eth0

#include <EtherSia.h>

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

    // Start Ethernet
    if (ether.begin(macAddress) == false) {
        Serial.println("Failed to get a global address");
    }
    
    Serial.print("Our link-local address is: ");
    ether.linkLocalAddress().println();
    
    if (tcp.setRemoteAddress(emonip, emonport)) {
        Serial.print("emoncms server IPv6 address : ");
        tcp.remoteAddress().println();
    }
    Serial.println("Ready.");
}

void loop()
{
    ether.receivePacket();
    
    if (!tcp.connected() && (long)(millis() - lastConnect) >= 10000) {  
        Serial.println();Serial.println();
        Serial.println(lastConnect);
        Serial.println("...Connecting emonserver.");
        
        //TCP three-ways handshake
        //tcp.connect() sends the SYN bit to the server
        tcp.connect();
        //ether.receivePacket() receives the SYNACK and tcp.havePacket() replies with an ACK
        while(!tcp.connected()){
          ether.receivePacket();
          tcp.havePacket();
        }

        //!!DATA transfer!!
        if(tcp.connected()){
          Serial.println("successfull connection");
          tcp.print("GET /emoncms/input/post.json?node=0&json={power:200}&apikey=");
          //if wish of an example without any data posting and only a time request, comment previous line and uncomment next one
          //tcp.print("GET /emoncms/time/local.json?&apikey=");
          tcp.println(emonapikey);
          tcp.send();
          while(!tcp.havePacket())ether.receivePacket();
          uint8_t *buf = tcp.payload();
          buf[tcp.payloadLength()] = '\0';
          Serial.print("Data: ");
          Serial.println((char*)(&(buf[0])));
          //tcp.disconnect() sends the FIN bit 
          tcp.disconnect();
        }     
        lastConnect = millis();
    }
    
}
