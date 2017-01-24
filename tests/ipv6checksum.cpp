/*

  Small tool to calculate the checksum of an IPv6 packet in a Hext file

*/

#include "EtherSia.h"
#include "hext.hh"

#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ipv6_checksum <filename.hext>\n");
        return -1;
    }

    HextFile input(argv[1]);
    IPv6Packet *packet = (IPv6Packet *)input.buffer;

    printf("    Input filename: %s\n", argv[1]);
    printf(" Input file length: %d\n", input.length);

    // Check the EtherType
    if (packet->etherType() != ETHER_TYPE_IPV6) {
        fprintf(stderr, "Error: Packet's EtherType is not 0x%4.4x\n", ETHER_TYPE_IPV6);
        return -1;
    }

    // Check the version header
    if (packet->version() != 6) {
        fprintf(stderr, "Error: Packet's IP version is not 6\n");
        return -1;
    }

    printf("     Packet length: %d\n", packet->length());
    printf("    Payload length: %d\n", packet->payloadLength());
    if (packet->length() != input.length) {
        fprintf(stderr, "Error: packet length != file length\n");
        fprintf(stderr, "Payload length should = 0x%4.4x\n", input.length - ETHER_HEADER_LEN - IP6_HEADER_LEN);
        return -1;
    }

    printf("   Packet Checksum: 0x%4.4x\n", packet->calculateChecksum());

    return 0;
}
