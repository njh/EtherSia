#include "EtherSia.h"
#include "util.h"

UDPSocket::UDPSocket(EtherSia &ether) : _ether(ether)
{
    _localPort = random(20000, 30000);
    _remoteAddress.setZero();
    _remotePort = 0;
}

UDPSocket::UDPSocket(EtherSia &ether, uint16_t localPort) : _ether(ether)
{
    _localPort = localPort;
    _remoteAddress.setZero();
    _remotePort = 0;
}

boolean UDPSocket::setRemoteAddress(const char *remoteAddress, uint16_t remotePort)
{
    _remotePort = remotePort;
    if (containsColon(remoteAddress)) {
        // Parse a human readable IPv6 Address string
        return _remoteAddress.fromString(remoteAddress);
    } else {
        // Lookup a hostname
        IPv6Address *address = _ether.lookupHostname(remoteAddress);
        if (address) {
            _remoteAddress = *address;
            return true;
        } else {
            // Fail
            return false;
        }
    }
}

boolean UDPSocket::setRemoteAddress(IPv6Address &remoteAddress, uint16_t remotePort)
{
    _remotePort = remotePort;
    _remoteAddress = remoteAddress;
    return true;
}

IPv6Address& UDPSocket::remoteAddress()
{
    return _remoteAddress;
}

uint16_t UDPSocket::remotePort()
{
    return _remotePort;
}

uint16_t UDPSocket::localPort()
{
    return _localPort;
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

void UDPSocket::send(const char *data)
{
    send((const uint8_t *)data, strlen(data));
}

void UDPSocket::send(const void *data, uint16_t len)
{
    IPv6Packet& packet = _ether.packet();
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    // FIXME: check it isn't too big
    memcpy((uint8_t*)udpHeader + UDP_HEADER_LEN, data, len);

    send(len);
}

void UDPSocket::send(uint16_t len)
{
    IPv6Packet& packet = _ether.packet();
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    packet.setDestination(_remoteAddress);
    udpHeader->destinationPort = htons(_remotePort);
    _ether.prepareSend();

    sendInternal(len);
}

void UDPSocket::sendReply(const char *data)
{
    sendReply((const uint8_t*)data, strlen(data));
}

void UDPSocket::sendReply(const void* data, uint16_t len)
{
    IPv6Packet& packet = _ether.packet();
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    memcpy((uint8_t*)udpHeader + UDP_HEADER_LEN, data, len);

    sendReply(len);
}

void UDPSocket::sendReply(uint16_t len)
{
    IPv6Packet& packet = _ether.packet();
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    udpHeader->destinationPort = udpHeader->sourcePort;
    _ether.prepareReply();

    sendInternal(len);
}

void UDPSocket::sendInternal(uint16_t length)
{
    IPv6Packet& packet = _ether.packet();
    struct udp_header *udpHeader = UDP_HEADER_PTR;
    uint16_t totalLen = UDP_HEADER_LEN + length;

    packet.setProtocol(IP6_PROTO_UDP);
    packet.setPayloadLength(totalLen);

    udpHeader->length = htons(totalLen);
    udpHeader->sourcePort = ntohs(_localPort);
    udpHeader->checksum = 0;
    udpHeader->checksum = htons(packet.calculateChecksum());

    _ether.send();
}

IPv6Address& UDPSocket::packetSource()
{
    return _ether.packet().source();
}

IPv6Address& UDPSocket::packetDestination()
{
    return _ether.packet().destination();
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
