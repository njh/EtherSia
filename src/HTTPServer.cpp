#include "EtherSia.h"
#include "util.h"

FlashStringMaker(HTTPServer, typeHtml, "text/html");
FlashStringMaker(HTTPServer, typePlain, "text/plain");
FlashStringMaker(HTTPServer, typeJson, "application/json");

FlashStringMaker(HTTPServer, status200, "200 OK");
FlashStringMaker(HTTPServer, status302, "302 Redirect");
FlashStringMaker(HTTPServer, status404, "404 Not Found");

const char PROGMEM HTTPServer::methodGet[] = "GET";
const char PROGMEM HTTPServer::methodPost[] = "POST";
const char PROGMEM HTTPServer::methodPut[] = "PUT";
const char PROGMEM HTTPServer::methodDelete[] = "DELETE";



HTTPServer::HTTPServer(EtherSia &ether, uint16_t localPort) : TCPServer(ether, localPort)
{
    _bodyPtr = NULL;
}

void HTTPServer::printStatus(const __FlashStringHelper* status)
{
    print(F("HTTP/1.0 "));
    println(status);
    println(F("Server: EtherSia"));
}

void HTTPServer::printHeaders(const __FlashStringHelper* contentType, const __FlashStringHelper* status)
{
    printStatus(status);
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

void HTTPServer::redirect(const __FlashStringHelper* location)
{
    printStatus(status302);
    print(F("Location: "));
    println(location);
    println();
    println(status302);
    sendReply();
}

boolean HTTPServer::checkRequest(const char* method, const __FlashStringHelper* path)
{
    char* payload = (char*)this->payload();
    uint16_t length = payloadLength();
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

    // Do the paths match?
    const char *matchpath = reinterpret_cast<const char *>(path);
    uint8_t maxlen = length - endOfMethod - 1;
    bool allowAny = false;
    _pathPtr = &payload[endOfMethod + 1];
    for(uint8_t i=0; i < maxlen; i++) {
        char match = pgm_read_byte(matchpath + i);

        // Have we got the end of the path section in the HTTP request?
        if (isWhitespace(_pathPtr[i]) || _pathPtr[i] == '?') {
            if (match == '\0') {
                // Both ended at the same time
                _pathPtr[i] = '\0';
                break;
            } else {
                // Path ended before the string we are trying to match against
                return false;
            }
        }

        // Allow anything after a '#' character
        if (match == '#')
            allowAny = true;

        // Allow any character to match a '?'
        if (match == '?' || allowAny)
            continue;

        // Do they match up?
        if (_pathPtr[i] != match)
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

    return payloadLength() - (_bodyPtr - (char*)payload());
}

boolean HTTPServer::bodyEquals(const char str[])
{
    if (_bodyPtr == NULL)
        return false;

    return strcmp(_bodyPtr, str) == 0;
}
