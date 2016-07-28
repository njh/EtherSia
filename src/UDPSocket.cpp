#include "EtherSia.h"
#include "util.h"

UDPSocket::UDPSocket(EtherSia &ether) : ether(ether)
{
    this->localPort = random(20000, 30000);
    this->remoteAddress.setZero();
    this->remotePort = 0;
}

UDPSocket::UDPSocket(EtherSia &ether, uint16_t localPort) : ether(ether)
{
    this->localPort = localPort;
    this->remoteAddress.setZero();
    this->remotePort = 0;
}

UDPSocket::UDPSocket(EtherSia &ether, IPv6Address &remoteAddress, uint16_t remotePort) : ether(ether)
{
    this->localPort = 10000 | remotePort;
    setRemoteAddress(remoteAddress, remotePort);
}

boolean UDPSocket::setRemoteAddress(const char *remoteAddress, uint16_t remotePort)
{
    this->remotePort = remotePort;
    if (containsColon(remoteAddress)) {
        // Parse a human readable IPv6 Address string
        return this->remoteAddress.fromString(remoteAddress);
    } else {
        // Lookup a hostname
        IPv6Address *address = ether.getHostByName(remoteAddress);
        if (address) {
            this->remoteAddress = *address;
            return true;
        } else {
            // Fail
            return false;
        }
    }
}

boolean UDPSocket::setRemoteAddress(IPv6Address &remoteAddress, uint16_t remotePort)
{
    this->remotePort = remotePort;
    this->remoteAddress = remoteAddress;
    return true;
}

IPv6Address& UDPSocket::getRemoteAddress()
{
    return remoteAddress;
}

uint16_t UDPSocket::getRemotePort()
{
    return remotePort;
}

boolean UDPSocket::havePacket()
{
    IPv6Packet *packet = ether.getPacket();

    if (!packet->isValid()) {
        return 0;
    }

    if (packet->protocol != IP6_PROTO_UDP) {
        // Wrong protocol
        return 0;
    }

    if (packetDestinationPort() != localPort) {
        // Wrong destination port
        return 0;
    }

    if (remotePort && packetSourcePort() != remotePort) {
        // Wrong source port
        return 0;
    }

    if (!ether.isOurAddress(packetDestination())) {
        // Wrong destination address
        return 0;
    }

    if (!remoteAddress.isZero() && packetSource() != remoteAddress) {
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

void UDPSocket::send(const uint8_t *data, uint16_t len)
{
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    // FIXME: check it isn't too big
    memcpy((uint8_t*)udpHeader + UDP_HEADER_LEN, data, len);

    send(len);
}

void UDPSocket::send(uint16_t len)
{
    IPv6Packet *packet = ether.getPacket();
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    // Setup the IPv6 packet header
    packet->length = htons(UDP_HEADER_LEN + len);
    packet->protocol = IP6_PROTO_UDP;
    packet->destination = remoteAddress;
    ether.prepareSend();

    // Set the UDP header
    udpHeader->length = packet->length;
    udpHeader->destinationPort = htons(remotePort);
    udpHeader->sourcePort = htons(localPort);
    udpHeader->checksum = 0;
    udpHeader->checksum = htons(packet->calculateChecksum());

    ether.send();
}

void UDPSocket::sendReply(const char *data)
{
    sendReply((const uint8_t*)data, strlen(data));
}

void UDPSocket::sendReply(const uint8_t* data, uint16_t len)
{
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    memcpy((uint8_t*)udpHeader + UDP_HEADER_LEN, data, len);

    sendReply(len);
}

void UDPSocket::sendReply(uint16_t len)
{
    IPv6Packet *packet = ether.getPacket();
    struct udp_header *udpHeader = UDP_HEADER_PTR;
    uint16_t destinationPort = udpHeader->destinationPort;
    uint16_t sourcePort = udpHeader->sourcePort;

    packet->length = htons(UDP_HEADER_LEN + len);
    ether.prepareReply();

    udpHeader->length = packet->length;
    udpHeader->destinationPort = sourcePort;
    udpHeader->sourcePort = destinationPort;
    udpHeader->checksum = 0;
    udpHeader->checksum = htons(packet->calculateChecksum());

    ether.send();
}

IPv6Address& UDPSocket::packetSource()
{
    return getPacket()->source;
}

IPv6Address& UDPSocket::packetDestination()
{
    return getPacket()->destination;
}

uint16_t UDPSocket::packetSourcePort()
{
    return ntohs(UDP_HEADER_PTR->sourcePort);
}

uint16_t UDPSocket::packetDestinationPort()
{
    return ntohs(UDP_HEADER_PTR->destinationPort);
}

uint16_t UDPSocket::packetLength()
{
    return ntohs(UDP_HEADER_PTR->length);
}

uint16_t UDPSocket::packetChecksum()
{
    return ntohs(UDP_HEADER_PTR->checksum);
}

uint8_t* UDPSocket::payload()
{
    return (uint8_t *)getPacket()->payload() + UDP_HEADER_LEN;
}

uint16_t UDPSocket::payloadLength()
{
    return packetLength() - UDP_HEADER_LEN;
}

boolean UDPSocket::payloadEquals(const char *str)
{
    return strncmp((char*)payload(), str, payloadLength()) == 0;
}

IPv6Packet* UDPSocket::getPacket()
{
    return ether.getPacket();
}
