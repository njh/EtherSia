#ifndef EtherSia_H
#define EtherSia_H

#include <Arduino.h>
#include <stdint.h>

#include "enc28j60.h"
#include "MACAddress.h"
#include "IPv6Address.h"
#include "IPv6Packet.h"
#include "UDPSocket.h"

#ifndef htons
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#endif

#ifndef ntohs
#define ntohs(x) htons(x)
#endif


enum {
    ADDRESS_TYPE_LINK_LOCAL = 1,
    ADDRESS_TYPE_GLOBAL,
    ADDRESS_TYPE_MULTICAST
};

/** How often to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_TIMEOUT    (3000)

/** How many times to send Router Solicitation (RS) packets */
#define ROUTER_SOLICITATION_ATTEMPTS   (4)


class EtherSia : public ENC28J60 {
public:
    EtherSia(int8_t cs);
    EtherSia(int8_t clk, int8_t miso, int8_t mosi, int8_t cs);

    boolean begin(const MACAddress *addr);

    void setGlobalAddress(IPv6Address *addr);
    void setGlobalAddress(const char* addr);
    IPv6Address* getGlobalAddress();
    IPv6Address* getLinkLocalAddress();

    IPv6Packet* receivePacket();
    IPv6Packet* getPacket();

    uint8_t isOurAddress(const IPv6Address *addr);

    void send();

    void prepareSend();
    void prepareReply();

protected:
    IPv6Address linkLocalAddr;
    IPv6Address globalAddr;

    MACAddress routerMac;

    uint8_t *buffer;
    uint16_t bufferLen;

    uint16_t icmp6Chksum();
    void icmp6SendNS(IPv6Address *target_addr);
    void icmp6ProcessPacket();
    void icmp6NSReply();
    void icmp6EchoReply();
    void icmp6SendRS();
    void icmp6ProcessRA();
    void icmp6ProcessPrefix(struct icmp6_prefix_information *pi);
    void icmp6PacketSend();
    uint8_t icmp6VerifyChecksum();
};

#endif
