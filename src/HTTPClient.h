/**
 * Header file for the HTTPClient class
 * @file HTTPClient.h
 */

#ifndef HTTPClient_H
#define HTTPClient_H

#include <stdint.h>
#include "TCPClient.h"


/**
 * Class for sending HTTP requests
 */
class HTTPClient : public TCPClient {

public:

    /**
     * Construct a HTTP client
     *
     * @param ether The Ethernet interface to attach the client to
     */
    HTTPClient(EtherSia &ether);


    void get(char *path);

    void head(char *path);


protected:

    /** A pointer to the a string for the method of the request */
    const char* _method;

    uint8_t status[3];
};


#endif
