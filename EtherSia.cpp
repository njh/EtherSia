#include <stdint.h>

#include "EtherSia.h"
#include "packet_headers.h"
#include "enc28j60.h"


EtherSia::EtherSia(int8_t cs) : ENC28J60(cs)
{
}


boolean EtherSia::begin(const uint8_t* macaddr)
{
    enc28j60_init(macaddr);

    // Calculate our link local address
    memset(link_local_addr, 0, 16);
    link_local_addr[0] = 0xfe;
    link_local_addr[1] = 0x80;
    set_eui_64(link_local_addr, macaddr);

    // FIXME: make this configurable
    buffer_len = 800;
    buffer = (uint8_t*)malloc(buffer_len);

    // Delay a 'random' amount to stop multiple nodes acting at the same time
    delay(macaddr[5] ^ 0x55);

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

void EtherSia::set_eui_64(uint8_t ipaddr[16], const uint8_t macaddr[6])
{
    ipaddr[8] = macaddr[0] ^ 0x02;
    ipaddr[9] = macaddr[1];
    ipaddr[10] = macaddr[2];
    ipaddr[11] = 0xff;
    ipaddr[12] = 0xfe;
    ipaddr[13] = macaddr[3];
    ipaddr[14] = macaddr[4];
    ipaddr[15] = macaddr[5];
}

boolean EtherSia::is_multicast_address(uint8_t addr[16])
{
    return addr[0] == 0xFF;
}

uint8_t EtherSia::is_our_address(uint8_t addr[16])
{
    return memcmp(addr, link_local_addr, 16) == 0;
}

void EtherSia::process_packet(uint16_t len)
{
    if (ETHER_HEADER->type != htons(ETHER_TYPE_IPV6)) {
        return;
    }

    Serial.println();

    Serial.print("LEN=");
    Serial.print(len, DEC);
    Serial.println();

    Serial.print("DST=");
    print_mac(ETHER_HEADER->dest);

    Serial.print("SRC=");
    print_mac(ETHER_HEADER->src);

#ifdef DEBUG
    if ((IP6_HEADER->ver_tc[0] >> 4) & 0xF != 6) {
        Serial.println("NOT 6");
        return;
    }
#endif

    Serial.print("length=");
    Serial.print(ntohs(IP6_HEADER->length), DEC);
    Serial.println();

    Serial.print("proto=");
    Serial.print(IP6_HEADER->proto, DEC);
    Serial.println();

    Serial.print("src=");
    print_address(IP6_HEADER->src);

    Serial.print("dst=");
    print_address(IP6_HEADER->dest);

    switch(IP6_HEADER->proto) {
    case IP6_PROTO_ICMP6:
        icmp6_process_packet(len);
        break;

    case IP6_PROTO_TCP:
        Serial.println(F("Got TCP packet"));
        break;

    case IP6_PROTO_UDP:
        Serial.println(F("Got UDP packet"));
        break;

    default:
        Serial.print(F("Unknown next header type: "));
        Serial.println(IP6_HEADER->proto, DEC);
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
    memcpy(IP6_HEADER->dest, IP6_HEADER->src, 16);
    memcpy(IP6_HEADER->src, link_local_addr, 16);

    memcpy(ETHER_HEADER->dest, ETHER_HEADER->src, 6);
    memcpy(ETHER_HEADER->src, enc_mac_addr, 6);
}

void EtherSia::ip6_packet_send()
{
    uint16_t len = ETHER_HEADER_LEN + IP6_HEADER_LEN + ntohs(IP6_HEADER->length);
    enc28j60_send(buffer, len);
}
