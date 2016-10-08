#include "EtherSia.h"
#include "util.h"

FlashStringMaker(HTTPServer, typeHtml, "text/html");
FlashStringMaker(HTTPServer, typePlain, "text/plain");
FlashStringMaker(HTTPServer, typeJson, "application/json");

FlashStringMaker(HTTPServer, status200, "200 OK");
FlashStringMaker(HTTPServer, status404, "404 Not Found");

const char PROGMEM HTTPServer::methodGet[] = "GET";
const char PROGMEM HTTPServer::methodPost[] = "POST";
const char PROGMEM HTTPServer::methodPut[] = "PUT";
const char PROGMEM HTTPServer::methodDelete[] = "DELETE";



HTTPServer::HTTPServer(EtherSia &ether, uint16_t localPort) : TCPServer(ether, localPort)
{
}

void HTTPServer::printHeaders(const __FlashStringHelper* contentType, const __FlashStringHelper* status)
{
      print(F("HTTP/1.0 "));
      println(status);
      print(F("Content-Type: "));
      println(contentType);
      println();
}

void HTTPServer::notFound()
{
   if (havePacket()) {
      printHeaders(typePlain, status404);
      println(status404);
      sendReply();
   }
}


boolean HTTPServer::checkRequest(const char* method, const __FlashStringHelper* path)
{
    char* payload = (char*)requestPayload();
    uint16_t length = requestLength();
    uint16_t endOfMethod;
    uint16_t endOfPath;

    // Is there a TCP request ready for us?
    if (!havePacket())
        return false;

    // Does the method match?
    endOfMethod = strlen_P(method);
    if (memcmp_P(payload, method, endOfMethod) != 0) {
        // Wrong method
        return false;
    }
    
    // Check there is a space after the method
    if (payload[endOfMethod] != ' ') {
        // No space after method
        return false;
    }
    
    // Search for the end of the path section
    for(endOfPath = endOfMethod + 1; endOfPath < length; endOfPath++) {
        if (payload[endOfPath] == ' ' || payload[endOfPath] == '?' || payload[endOfPath] == '\0') {
            payload[endOfPath] = '\0';
            break;
        }
    }

    if (endOfPath == length) {
        // Failed to find end of path
        return false;
    }

    // Do the paths match?
    const char *charpath = reinterpret_cast<const char *>(path);
    if (strcmp_P(payload + endOfMethod + 1, charpath) != 0) {
        // Path mismatch
        return false;
    }

    return true;
}
