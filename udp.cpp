
#include "EtherSia.h"


void EtherSia::udp_process_packet(uint16_t len)
{
    uint16_t dest_port = ntohs(UDP_HEADER->dest_port);

    if (!is_our_address(IP6_HEADER->dest)) {
        return;
    }

    if (!udp_verify_checksum()) {
        Serial.println(F("UDP checksum error."));
        return;
    }

    if (this->udp_callback && dest_port == this->udp_port) {
        this->udp_callback(
            dest_port,
            IP6_HEADER->src,
            UDP_PAYLOAD_PTR,
            ntohs(UDP_HEADER->length) - UDP_HEADER_LEN
        );
    }
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
