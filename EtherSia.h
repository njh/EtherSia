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




class EtherSia : public ENC28J60 {
public:
    EtherSia(int8_t cs);
    EtherSia(int8_t clk, int8_t miso, int8_t mosi, int8_t cs);

    boolean begin(const MACAddress *addr);

    IPv6Packet* receivePacket();
    IPv6Packet* getPacket();

    uint8_t is_our_address(const IPv6Address *addr);

    void send();

    void prepareSend();
    void prepareReply();

protected:
    IPv6Address link_local_addr;
    IPv6Address global_addr;

    MACAddress router_mac;

    uint8_t *buffer;
    uint16_t buffer_len;

    uint16_t icmp6_chksum();
    void icmp6_process_packet(uint16_t len);
    void icmp6_send_ns(IPv6Address *target_addr);
    void icmp6_ns_reply();
    void icmp6_echo_reply();
    void icmp6_send_rs();
    void icmp6_process_ra();
    void icmp6_process_prefix(struct icmp6_prefix_information *pi, MACAddress *router_mac_ptr);
    void icmp6_packet_send();
    uint8_t icmp6_verify_checksum();
};

#endif
