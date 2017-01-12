/**
 * Header file for the HTTPServer class
 * @file HTTPServer.h
 */

#ifndef HTTPServer_H
#define HTTPServer_H

#include <stdint.h>
#include "TCPServer.h"


/**
 * Class for responding to HTTP requests
 */
class HTTPServer : public TCPServer {

public:

    /**
     * Construct a HTTP server, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the server to
     * @param localPort The local HTTP port number to listen on
     */
    HTTPServer(EtherSia &ether, uint16_t localPort=80);

    /**
     * Check if request is of type GET and matches path
     *
     * @param path The path to check for (use the F("") macro)
     * * If the path contains a '?' it will match any character in the request patj
     * * If the path contains a '#' it will match the rest of the path
     * @return True if the method and path matches
     */
    inline boolean isGet(const __FlashStringHelper* path) { return checkRequest(methodGet, path); }

    /**
     * Check if request is of type POST and matches path
     *
     * @param path The path to check for (use the F("") macro)
     * * If the path contains a '?' it will match any character in the request patj
     * * If the path contains a '#' it will match the rest of the path
     * @return True if the method and path matches
     */
    inline boolean isPost(const __FlashStringHelper* path) { return checkRequest(methodPost, path); }

    /**
     * Check if request is of type PUT and matches path
     *
     * @param path The path to check for (use the F("") macro)
     * * If the path contains a '?' it will match any character in the request patj
     * * If the path contains a '#' it will match the rest of the path
     * @return True if the method and path matches
     */
    inline boolean isPut(const __FlashStringHelper* path) { return checkRequest(methodPut, path); }

    /**
     * Check if request is of type DELETE and matches path
     *
     * @param path The path to check for (use the F("") macro)
     * * If the path contains a '?' it will match any character in the request patj
     * * If the path contains a '#' it will match the rest of the path
     * @return True if the method and path matches
     */
    inline boolean isDelete(const __FlashStringHelper* path) { return checkRequest(methodDelete, path); }

    /**
     * Return 404 Not Found response to the client
     */
    void notFound();

    /**
     * Return redirect response to the client
     * @param location the address to redirect the client to. Use the F() macro.
     */
    void redirect(const __FlashStringHelper* location);

    /**
     * Write HTTP status line into the packet buffer
     *
     * @param status A flash string for the status code and message. Use the F() macro or one of:
     *  * @ref status200 (default)
     *  * @ref status302
     *  * @ref status404
     */
    void printStatus(const __FlashStringHelper* status=status200);

    /**
     * Write HTTP response header into the packet buffer
     *
     * @param contentType An flash string for the Content-Type header. Use the F() macro or one of:
     *  * @ref typeHtml
     *  * @ref typePlain (default)
     *  * @ref typeJson
     * @param status A flash string for the status code and message. Use the F() macro or one of:
     *  * @ref status200 (default)
     *  * @ref status302
     *  * @ref status404
     */
    void printHeaders(const __FlashStringHelper* contentType=typePlain, const __FlashStringHelper* status=status200);

    /**
     * Get the body section of the HTTP request as a C string
     *
     * @warning Some browsers (eg Safari) send the HTTP request headers
     * and the request body in seperate TCP segments. In this case EtherSia
     * is unable to see the response body.
     *
     * @return Pointer to the body or NULL if the body is empty
     */
    inline char* body() { return _bodyPtr; }

    /**
     * Get the path of the HTTP request
     *
     * @return the path for the HTTP request as a C string
     */
    inline char* path() { return _pathPtr; }

    /**
     * Get the length of the HTTP body
     *
     * @warning Annoyingly, some browsers (eg Safari) send the HTTP request headers
     * and the request body in seperate TCP segments. In this case EtherSia
     * is unable to see the response body.
     *
     * @return The length of the HTTP body (or 0 if there isn't one)
     */
    uint16_t bodyLength();

    /**
     * Check if request body equals the given C string
     *
     * @warning Annoyingly, some browsers (eg Safari) send the HTTP request headers
     * and the request body in seperate TCP segments. In this case EtherSia
     * is unable to see the response body.
     *
     * @param str The string to compare to
     * @return true if the body and string are equal
     */
    boolean bodyEquals(const char str[]);


    static const __FlashStringHelper* typeHtml;      /**< String for 'text/html' content type */
    static const __FlashStringHelper* typePlain;     /**< String for 'text/plain' content type */
    static const __FlashStringHelper* typeJson;      /**< String for 'application/json' content type */

    static const __FlashStringHelper* status200;     /**< String for '200 OK' status code */
    static const __FlashStringHelper* status302;     /**< String for '302 Redirect' status code */
    static const __FlashStringHelper* status404;     /**< String for '404 Not Found' status code */

protected:

    /** A pointer to the start of the body section */
    char* _bodyPtr;

    /** A pointer path string for current request */
    char* _pathPtr;

    /**
     * Check if request is of method and path matches the incoming request
     *
     * @param method The method to check for (must be in programme memory)
     * @param path The path to check for (use the F() macro)
     * @return True if the method and path matches
     */
    boolean checkRequest(const char* method, const __FlashStringHelper* path);

    static const char PROGMEM methodGet[];     /**< String for GET method */
    static const char PROGMEM methodPost[];    /**< String for POST method */
    static const char PROGMEM methodPut[];     /**< String for PUT method */
    static const char PROGMEM methodDelete[];  /**< String for DELETE method */
};


#endif
