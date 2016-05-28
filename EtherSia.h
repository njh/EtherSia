#ifndef EtherSia_H
#define EtherSia_H

#include <Arduino.h>

#include "enc28j60.h"

#ifndef htons
#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#endif

#ifndef ntohs
#define ntohs(x) htons(x)
#endif


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

    void ip6_packet_send();
protected:
    uint8_t link_local_addr[16];

    uint8_t *buffer;
    uint16_t buffer_len;

    void process_packet(uint16_t len);

    void convert_buffer_to_reply();

    uint16_t icmp6_chksum();
    void process_icmp6(uint16_t len);
    void icmp6_ns_reply();
    void icmp6_echo_reply();

};

#endif
