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
    _bodyPtr = NULL;
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
    uint16_t pos;

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
    for(pos = endOfMethod + 1; pos < length; pos++) {
        if (payload[pos] == ' ' || payload[pos] == '?' || payload[pos] == '\0') {
            payload[pos] = '\0';
            break;
        }
    }

    if (pos == length) {
        // Failed to find end of path
        return false;
    }

    // Do the paths match?
    const char *charpath = reinterpret_cast<const char *>(path);
    if (strcmp_P(payload + endOfMethod + 1, charpath) != 0) {
        // Path mismatch
        return false;
    }

    // Find the start of the body section
    _bodyPtr = NULL;
    for(pos = endOfMethod+1; pos < length-1; pos++) {
        if (payload[pos] == '\n' && (payload[pos-2] == '\n' || payload[pos-1] == '\n')) {
            // Store the location of the start of the body
            _bodyPtr = &payload[pos+1];

            // For convenience NULL-terminate the body
            payload[length] = '\0';
            break;
        }
    }

    return true;
}

uint16_t HTTPServer::bodyLength()
{
    if (_bodyPtr == NULL)
        return 0;

    return requestLength() - (_bodyPtr - (char*)requestPayload());
}

boolean HTTPServer::bodyEquals(const char str[])
{
    if (_bodyPtr == NULL)
        return false;

    return strcmp(_bodyPtr, str) == 0;
}
