#ifndef UDPSocket_H
#define UDPSocket_H

#include <stdint.h>
#include "IPv6Packet.h"

class EtherSia;

class UDPSocket {

public:

    UDPSocket(EtherSia *ether, uint16_t port);
    UDPSocket(EtherSia *ether, IPv6Address *destination, uint16_t port);

    void setDestination(IPv6Address *address, uint16_t port);

    bool havePacket();
    bool verifyChecksum();
    
    void send(const char *data);
    void send(const uint8_t *data=NULL, uint16_t length=0);
    void sendReply(const char *data);
    void sendReply(const uint8_t *data=NULL, uint16_t length=0);

    IPv6Address* packetSource();
    IPv6Address* packetDestination();
    uint16_t packetSourcePort();
    uint16_t packetDestinationPort();
    uint16_t packetLength();
    uint16_t packetChecksum();

    uint8_t* payload();
    uint16_t payloadLength();
    bool payloadEquals(const char *str);
    
protected:

    IPv6Packet* getPacket();

    EtherSia *ether;
    IPv6Address destAddress;
    uint16_t destPort;
};



struct udp_header {
    uint16_t srcPort;
    uint16_t destPort;
    uint16_t length;
    uint16_t checksum;
} __attribute__((__packed__));
#define UDP_HEADER_LEN            (8)
#define UDP_HEADER_PTR            ((struct udp_header*)(getPacket()->payload()))

static_assert(sizeof(struct udp_header) == UDP_HEADER_LEN, "Size is not correct");


#endif
