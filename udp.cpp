
#include "EtherSia.h"


void EtherSia::udp_process_packet(uint16_t len)
{
    uint16_t dest_port = ntohs(UDP_HEADER->dest_port);

    if (!is_our_address(&IP6_HEADER->dest)) {
        return;
    }

    if (!udp_verify_checksum()) {
        Serial.println(F("UDP checksum error."));
        return;
    }

    if (this->udp_callback && dest_port == this->udp_port) {
        this->udp_callback(
            dest_port,
            &IP6_HEADER->src,
            UDP_PAYLOAD_PTR,
            ntohs(UDP_HEADER->length) - UDP_HEADER_LEN
        );
    }
}

void EtherSia::udpSend(uint16_t port, const char *data)
{
    udpSend(port, (const uint8_t *)data, strlen(data));
}

void EtherSia::udpSend(uint16_t port, const uint8_t *data, uint16_t len)
{
    if (data) {
        // FIXME: check it isn't too big
        memcpy(UDP_PAYLOAD_PTR, data, len);
    }

    // Setup the Ethernet Header
    ETHER_HEADER->src = enc_mac_addr;
    ETHER_HEADER->dest = router_mac;
    ETHER_HEADER->type = htons(ETHER_TYPE_IPV6);

    // Setup the IPv6 packet header
    memset(IP6_HEADER, 0, IP6_HEADER_LEN);
    IP6_HEADER->ver_tc = 0x60;
    IP6_HEADER->length = htons(UDP_HEADER_LEN + len);
    IP6_HEADER->proto = IP6_PROTO_UDP;
    IP6_HEADER->hop_limit = DEFAULT_HOP_LIMIT;
    IP6_HEADER->dest = dest_addr;
    if (dest_addr.isLinkLocal()) {
        IP6_HEADER->src = link_local_addr;
    } else {
        IP6_HEADER->src = global_addr;
    }

    // Set the UDP header
    UDP_HEADER->length = IP6_HEADER->length;
    UDP_HEADER->dest_port = htons(port);
    UDP_HEADER->src_port = UDP_HEADER->dest_port;
    UDP_HEADER->checksum = 0;
    UDP_HEADER->checksum = htons(ip6_calculate_checksum());

    send();
}

void EtherSia::udpSendReply(const char *data)
{
    udpSendReply(data, strlen(data));
}

void EtherSia::udpSendReply(const char *data, uint16_t len)
{
    uint16_t dest_port = UDP_HEADER->dest_port;
    uint16_t src_port = UDP_HEADER->src_port;

    if (data) {
        memcpy(UDP_PAYLOAD_PTR, data, len);
    }

    prepareReply();

    UDP_HEADER->length = htons(UDP_HEADER_LEN + len);
    IP6_HEADER->length = UDP_HEADER->length;
    UDP_HEADER->dest_port = src_port;
    UDP_HEADER->src_port = dest_port;
    UDP_HEADER->checksum = 0;
    UDP_HEADER->checksum = htons(ip6_calculate_checksum());

    send();
}

uint8_t EtherSia::udp_verify_checksum()
{
    uint16_t packet_checksum = ntohs(UDP_HEADER->checksum);

    // Set field in packet to 0 before calculating the checksum
    UDP_HEADER->checksum = 0;

    // Does the calculated checksum equal the checksum in the packet?
    return ip6_calculate_checksum() == packet_checksum;
}

void EtherSia::udp_listen(UdpServerCallback callback, uint16_t port)
{
    // FIXME: allow listening on multiple ports?
    this->udp_callback = callback;
    this->udp_port = port;
}
