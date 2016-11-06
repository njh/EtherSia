#include "EtherSia.h"
#include "util.h"

UDPSocket::UDPSocket(EtherSia &ether) : Socket(ether)
{
}

UDPSocket::UDPSocket(EtherSia &ether, uint16_t localPort) : Socket(ether, localPort)
{
}

boolean UDPSocket::havePacket()
{
    IPv6Packet& packet = _ether.packet();

    if (!_ether.bufferContainsReceived()) {
        return 0;
    }

    if (packet.protocol() != IP6_PROTO_UDP) {
        // Wrong protocol
        return 0;
    }

    if (packetDestinationPort() != _localPort) {
        // Wrong destination port
        return 0;
    }

    if (_remotePort && packetSourcePort() != _remotePort) {
        // Wrong source port
        return 0;
    }

    if (!_ether.isOurAddress(packetDestination())) {
        // Wrong destination address
        return 0;
    }

    if (!_remoteAddress.isZero() && packetSource() != _remoteAddress) {
        // Wrong source address
        return 0;
    }

    // The packet in the buffer is valid for this socket
    return 1;
}

void UDPSocket::sendInternal(uint16_t length, boolean isReply)
{
    IPv6Packet& packet = _ether.packet();
    struct udp_header *udpHeader = UDP_HEADER_PTR;
    uint16_t totalLen = UDP_HEADER_LEN + length;

    packet.setProtocol(IP6_PROTO_UDP);
    packet.setPayloadLength(totalLen);

    udpHeader->length = htons(totalLen);
    if (isReply) {
        udpHeader->destinationPort = udpHeader->sourcePort;
    } else {
        udpHeader->destinationPort = ntohs(_remotePort);
    }
    udpHeader->sourcePort = ntohs(_localPort);
    udpHeader->checksum = 0;
    udpHeader->checksum = htons(packet.calculateChecksum());

    _ether.send();
}

uint16_t UDPSocket::packetSourcePort()
{
    IPv6Packet& packet = _ether.packet();
    return ntohs(UDP_HEADER_PTR->sourcePort);
}

uint16_t UDPSocket::packetDestinationPort()
{
    IPv6Packet& packet = _ether.packet();
    return ntohs(UDP_HEADER_PTR->destinationPort);
}

uint8_t* UDPSocket::payload()
{
    IPv6Packet& packet = _ether.packet();
    return (uint8_t *)(packet.payload()) + UDP_HEADER_LEN;
}

uint16_t UDPSocket::payloadLength()
{
    IPv6Packet& packet = _ether.packet();
    return ntohs(UDP_HEADER_PTR->length) - UDP_HEADER_LEN;
}

boolean UDPSocket::payloadEquals(const char *str)
{
    return strncmp((char*)payload(), str, payloadLength()) == 0;
}
