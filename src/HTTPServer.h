/**
 * Header file for the HTTPServer class
 * @file HTTPServer.h
 */

#ifndef HTTPServer_H
#define HTTPServer_H

#include <stdint.h>
#include "TCPServer.h"

class EtherSia;

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
     * Check if a TCP packet is available to be processed on this server
     * @return true if there is a valid packet has been received for this server
     */
    boolean isGet(const char* path);


    void notFound();

};


#endif
