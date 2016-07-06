#ifndef EtherSia_H
#define EtherSia_H

#include <Arduino.h>
#include <stdint.h>

#include "enc28j60.h"
#include "MACAddress.h"
#include "IPv6Address.h"
#include "packet_headers.h"

#ifndef htons
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#endif

#ifndef ntohs
#define ntohs(x) htons(x)
#endif
#include "IPv6Packet.h"


enum {
    ADDRESS_TYPE_LINK_LOCAL = 1,
    ADDRESS_TYPE_GLOBAL,
    ADDRESS_TYPE_MULTICAST
};


/** This type definition defines the structure of a UDP server event handler callback funtion */
typedef void (*UdpServerCallback)(
    uint16_t dest_port,        ///< Port the packet was sent to
    const IPv6Address *src_ip, ///< IP address of the sender
    char *data,                ///< UDP payload data
    uint16_t len);             ///< Length of the payload data


class EtherSia : public ENC28J60 {
public:
    EtherSia(int8_t cs);
    EtherSia(int8_t clk, int8_t miso, int8_t mosi, int8_t cs);

    boolean begin(const MACAddress *addr);

    void loop();
    IPv6Packet* receivePacket();
    IPv6Packet* getPacket();

    UDPPacket* getUDPPacket(uint16_t port);

    uint8_t is_our_address(const IPv6Address *addr);

    void send();

    void prepareReply();

    bool setDestinationAddress(const char *addr);
    IPv6Address* getDestinationAddress();

    void udp_listen(UdpServerCallback callback, uint16_t port);
    void udpSendReply(const char *data);
    void udpSendReply(const char *data, uint16_t len);

    void udpSend(uint16_t port, const char *data);
    void udpSend(uint16_t port, const uint8_t *data, uint16_t len);

protected:
    IPv6Address link_local_addr;
    IPv6Address global_addr;

    MACAddress router_mac;

    IPv6Address dest_addr;
    MACAddress dest_mac;

    uint8_t *buffer;
    uint16_t buffer_len;

    uint16_t udp_port;
    UdpServerCallback udp_callback;

    void process_packet(uint16_t len);

    uint16_t ip6_calculate_checksum();

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

    void udp_process_packet(uint16_t len);
    uint8_t udp_verify_checksum();
};

#endif
