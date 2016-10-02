#include "EtherSia.h"
#include "util.h"

HTTPServer::HTTPServer(EtherSia &ether, uint16_t localPort) : TCPServer(ether, localPort)
{
}


boolean HTTPServer::isGet(const char* path)
{
    if (havePacket()) {
        
        Serial.println("havePacket");
        
        return true;
    }
    
    return false;
}


void HTTPServer::notFound()
{

}
