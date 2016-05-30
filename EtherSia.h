#ifndef EtherSia_H
#define EtherSia_H

#include <Arduino.h>
#include <stdint.h>

#include "enc28j60.h"
#include "packet_headers.h"

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


/** This type definition defines the structure of a UDP server event handler callback funtion */
typedef void (*UdpServerCallback)(
    uint16_t dest_port,    ///< Port the packet was sent to
    const uint8_t *src_ip, ///< IP address of the sender
    const char *data,      ///< UDP payload data
    uint16_t len);         ///< Length of the payload data


class EtherSia : public ENC28J60 {
public:
    EtherSia(int8_t cs);
    EtherSia(int8_t clk, int8_t miso, int8_t mosi, int8_t cs);

    boolean begin(const uint8_t* macaddr);

    void loop();

    void set_eui_64(uint8_t ipaddr[16], const uint8_t macaddr[6]);
    boolean is_multicast_address(uint8_t addr[16]);
    uint8_t is_our_address(uint8_t addr[16]);

    void print_mac(const uint8_t mac[6]);
    void print_address(const uint8_t addr[16]);

    void udp_listen(UdpServerCallback callback, uint16_t port);

protected:
    uint8_t link_local_addr[16];
    uint8_t global_addr[16];
    uint8_t router_mac[6];

    uint8_t *buffer;
    uint16_t buffer_len;
    
    uint16_t udp_port;
    UdpServerCallback udp_callback;

    void process_packet(uint16_t len);

    uint16_t ip6_calculate_checksum();

    void convert_buffer_to_reply();
    void ip6_packet_send();

    uint16_t icmp6_chksum();
    void icmp6_process_packet(uint16_t len);
    void icmp6_ns_reply();
    void icmp6_echo_reply();
    void icmp6_send_rs();
    void icmp6_process_ra();
    void icmp6_process_prefix(struct icmp6_prefix_information *pi, uint8_t *router_mac_ptr);

    void udp_process_packet(uint16_t len);
    uint8_t udp_verify_checksum();
};

#endif
