#include <stdint.h>

#include "EtherSia.h"
#include "packet_headers.h"
#include "enc28j60.h"


static void
set_linklocal_addr(uint8_t ipaddr[16], const uint8_t macaddr[6])
{
    memset(ipaddr, 0, 16);

    ipaddr[0] = 0xfe;
    ipaddr[1] = 0x80;

    memcpy(ipaddr + 8, macaddr, 3);
    ipaddr[8] ^= 0x02;
    ipaddr[11] = 0xff;
    ipaddr[12] = 0xfe;
    memcpy(ipaddr + 13, macaddr + 3, 3);
}

EtherSia::EtherSia(int8_t cs) : ENC28J60(cs)
{
}


boolean EtherSia::begin(const uint8_t* macaddr)
{
    enc28j60_init(macaddr);

    set_linklocal_addr(link_local_addr, macaddr);
    
    // FIXME: make this configurable
    buffer_len = 800;
    buffer = (uint8_t*)malloc(buffer_len);

    return true;
}


static void printHex(byte byte)
{
    char str[3];
    str[0] = (byte >> 4) & 0x0f;
    str[1] = byte & 0x0f;
    str[2] = '\0';

    for (int i=0; i<2; i++) {
        // base for converting single digit numbers to ASCII is 48
        // base for 10-16 to become lower-case characters a-f is 87
        if (str[i] > 9) str[i] += 39;
        str[i] += 48;
    }
    Serial.print(str);
}

void EtherSia::print_mac(const uint8_t mac[6])
{
    for (byte i = 0; i < 6; ++i) {
        printHex(mac[i]);
        if (i < 5)
            Serial.print(':');
    }
    Serial.println();
}

void EtherSia::print_address(const uint8_t addr[16])
{
    for (byte i = 0; i < 16; ++i) {
        printHex(addr[i]);
        if (i % 2 == 1 && i < 15)
            Serial.print(':');
    }
    Serial.println();
}


void EtherSia::process_packet(uint16_t len)
{
    struct ether_header *header = (struct ether_header*)buffer;
    struct ip6_header *ip6 = (struct ip6_header *)(buffer + sizeof(struct ether_header));
    if (header->type != htons(ETHERTYPE_IPV6)) {
        return;
    }

    Serial.println();

    Serial.print("LEN=");
    Serial.print(len, DEC);
    Serial.println();

    Serial.print("DST=");
    print_mac(header->dest);

    Serial.print("SRC=");
    print_mac(header->src);

#ifdef DEBUG
    if ((ip6->ver_tc[0] >> 4) & 0xF != 6) {
        Serial.println("NOT 6");
        return;
    }
#endif

    Serial.print("length=");
    Serial.print(ntohs(ip6->length), DEC);
    Serial.println();

    Serial.print("proto=");
    Serial.print(ip6->proto, DEC);
    Serial.println();

    Serial.print("src=");
    print_address(ip6->src);

    Serial.print("dst=");
    print_address(ip6->dest);

    switch(ip6->proto) {
    case IP6_PROTO_ICMP6:
        process_icmp6(len);
        break;

    case IP6_PROTO_TCP:
        Serial.println(F("Got TCP packet"));
        break;

    case IP6_PROTO_UDP:
        Serial.println(F("Got UDP packet"));
        break;

    default:
        Serial.print(F("Unknown next header type: "));
        Serial.println(ip6->proto, DEC);
        break;
    }
}

void EtherSia::loop()
{
    int len = enc28j60_read(buffer, buffer_len);

    if (len) {
        process_packet(len);
    }
}

void EtherSia::convert_buffer_to_reply()
{
    struct ether_header *eth = (struct ether_header*)buffer;
    struct ip6_header *ip6 = (struct ip6_header *)(buffer + 14);

    memcpy(ip6->dest, &ip6->src, 16);
    memcpy(ip6->src, &link_local_addr, 16);

    memcpy(eth->dest, eth->src, 6);
    memcpy(eth->src, &enc_mac_addr, 6);
}
