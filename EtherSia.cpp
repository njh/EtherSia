#include "EtherSia.h"


EtherSia::EtherSia(int8_t cs) : ENC28J60(cs)
{
}


boolean EtherSia::begin(const MACAddress *macaddr)
{
    enc28j60_init(macaddr);

    // Calculate our link local address
    link_local_addr.setLinkLocalPrefix();
    link_local_addr.setEui64(macaddr);

    // FIXME: make this configurable
    buffer_len = 800;
    buffer = (uint8_t*)malloc(buffer_len);

    udp_port = 0;
    udp_callback = NULL;

    // Delay a 'random' amount to stop multiple nodes acting at the same time
    delay((*macaddr)[5] ^ 0x55);

    // Send link local Neighbour Solicitation for Duplicate Address Detection
    icmp6_send_ns(&link_local_addr);

    return true;
}

uint8_t EtherSia::is_our_address(const IPv6Address *addr)
{
    if (*addr == link_local_addr) {
        return ADDRESS_TYPE_LINK_LOCAL;
    } else if (*addr == global_addr) {
        return ADDRESS_TYPE_GLOBAL;
    } else {
        return 0;
    }
}

// This function comes from Contiki's uip6.c
static uint16_t chksum(uint16_t sum, const uint8_t *data, uint16_t len)
{
    uint16_t t;
    const uint8_t *dataptr;
    const uint8_t *last_byte;

    dataptr = data;
    last_byte = data + len - 1;

    while(dataptr < last_byte) {   /* At least two more bytes */
        t = (dataptr[0] << 8) + dataptr[1];
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
        dataptr += 2;
    }

    if(dataptr == last_byte) {
        t = (dataptr[0] << 8) + 0;
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
    }

    /* Return sum in host byte order. */
    return sum;
}

// This function is derived from Contiki's uip6.c / upper_layer_chksum()
uint16_t EtherSia::ip6_calculate_checksum()
{
    /* First sum pseudoheader. */
    /* IP protocol and length fields. This addition cannot carry. */
    volatile uint16_t newsum = ntohs(IP6_HEADER->length) + IP6_HEADER->proto;

    /* Sum IP source and destination addresses. */
    newsum = chksum(newsum, (uint8_t *)(IP6_HEADER->src), 32);

    /* Sum the payload header and data */
    uint8_t *payload = (uint8_t *)(this->buffer + IP6_HEADER_OFFSET + IP6_HEADER_LEN);
    newsum = chksum(newsum, payload, ntohs(IP6_HEADER->length));

    return ~newsum;
}

void EtherSia::process_packet(uint16_t len)
{
    if (ETHER_HEADER->type != htons(ETHER_TYPE_IPV6)) {
        return;
    }

#ifdef DEBUG
    if ((IP6_HEADER->ver_tc[0] >> 4) & 0xF != 6) {
        Serial.println("NOT 6");
        return;
    }
#endif

    switch(IP6_HEADER->proto) {
    case IP6_PROTO_ICMP6:
        icmp6_process_packet(len);
        break;

    case IP6_PROTO_TCP:
        Serial.println(F("Got TCP packet"));
        break;

    case IP6_PROTO_UDP:
        udp_process_packet(len);
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
    } else if (global_addr[0] == 0x00) {
        static unsigned long nextRouterSolicitation = millis();
        if ((long)(millis() - nextRouterSolicitation) >= 0) {
            icmp6_send_rs();
            nextRouterSolicitation = millis() + 4000;
        }
    }
}

void EtherSia::convert_buffer_to_reply()
{
    IPv6Address *reply_src_addr;

    if (is_our_address(&IP6_HEADER->dest) == ADDRESS_TYPE_GLOBAL) {
        reply_src_addr = &global_addr;
    } else {
        reply_src_addr = &link_local_addr;
    }

    IP6_HEADER->dest = IP6_HEADER->src;
    IP6_HEADER->src = *reply_src_addr;

    ETHER_HEADER->dest = ETHER_HEADER->src;
    ETHER_HEADER->src = enc_mac_addr;
}

void EtherSia::ip6_packet_send()
{
    uint16_t len = ETHER_HEADER_LEN + IP6_HEADER_LEN + ntohs(IP6_HEADER->length);
    enc28j60_send(buffer, len);
}
