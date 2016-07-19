#include "EtherSia.h"
#include "util.h"

UDPSocket::UDPSocket(EtherSia *ether)
{
    this->ether = ether;
    this->localPort = random(20000, 30000);
}

UDPSocket::UDPSocket(EtherSia *ether, uint16_t localPort)
{
    this->ether = ether;
    this->localPort = localPort;
}

UDPSocket::UDPSocket(EtherSia *ether, IPv6Address *remoteAddress, uint16_t remotePort)
{
    this->ether = ether;
    this->localPort = 10000 + remotePort;
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
        IPv6Address *addr = ether->getHostByName(remoteAddress);
        if (addr) {
            this->remoteAddress = *addr;
            return true;
        } else {
            // Fail
            return false;
        }
    }
}

boolean UDPSocket::setRemoteAddress(IPv6Address *remoteAddress, uint16_t remotePort)
{
    this->remotePort = remotePort;
    this->remoteAddress = *remoteAddress;
    return true;
}

IPv6Address* UDPSocket::getRemoteAddress()
{
    return &remoteAddress;
}

uint16_t UDPSocket::getRemotePort()
{
    return remotePort;
}

boolean UDPSocket::havePacket()
{
    IPv6Packet* packet = ether->getPacket();

    if (!packet->isValid()) {
        return 0;
    }

    if (packet->proto != IP6_PROTO_UDP) {
        // Wrong protocol
        return 0;
    }

    if (packetDestinationPort() != localPort) {
        // Wrong port
        return 0;
    }

    if (!ether->isOurAddress(packetDestination())) {
        // Wrong destination
        return 0;
    }

    if (!verifyChecksum()) {
        Serial.println(F("UDP checksum error."));
        return 0;
    }

    // The packet in the buffer is valid for this socket
    return 1;
}

boolean UDPSocket::verifyChecksum()
{
    IPv6Packet *packet = ether->getPacket();
    struct udp_header *udpHeader = UDP_HEADER_PTR;
    uint16_t packetChecksum = ntohs(udpHeader->checksum);

    // Set field in packet to 0 before calculating the checksum
    udpHeader->checksum = 0;

    // Does the calculated checksum equal the checksum in the packet?
    return packet->calculateChecksum() == packetChecksum;
}

void UDPSocket::send(const char *data)
{
    send((const uint8_t *)data, strlen(data));
}

void UDPSocket::send(const uint8_t *data, uint16_t len)
{
    IPv6Packet *packet = ether->getPacket();
    struct udp_header *udpHeader = UDP_HEADER_PTR;

    if (data) {
        // FIXME: check it isn't too big
        memcpy((uint8_t*)udpHeader + UDP_HEADER_LEN, data, len);
    }

    // Setup the IPv6 packet header
    packet->length = htons(UDP_HEADER_LEN + len);
    packet->proto = IP6_PROTO_UDP;
    packet->dest = remoteAddress;
    ether->prepareSend();

    // Set the UDP header
    udpHeader->length = packet->length;
    udpHeader->destPort = htons(remotePort);
    udpHeader->srcPort = htons(localPort);
    udpHeader->checksum = 0;
    udpHeader->checksum = htons(packet->calculateChecksum());

    ether->send();
}

void UDPSocket::sendReply(const char *data)
{
    sendReply((const uint8_t*)data, strlen(data));
}

void UDPSocket::sendReply(const uint8_t* data, uint16_t len)
{
    IPv6Packet *packet = ether->getPacket();
    struct udp_header *udpHeader = UDP_HEADER_PTR;
    uint16_t destPort = udpHeader->destPort;
    uint16_t srcPort = udpHeader->srcPort;

    if (data) {
        memcpy((uint8_t*)udpHeader + UDP_HEADER_LEN, data, len);
    }

    packet->length = htons(UDP_HEADER_LEN + len);
    ether->prepareReply();

    udpHeader->length = packet->length;
    udpHeader->destPort = srcPort;
    udpHeader->srcPort = destPort;
    udpHeader->checksum = 0;
    udpHeader->checksum = htons(packet->calculateChecksum());

    ether->send();
}

IPv6Address* UDPSocket::packetSource()
{
    return &getPacket()->src;
}

IPv6Address* UDPSocket::packetDestination()
{
    return &getPacket()->dest;
}

uint16_t UDPSocket::packetSourcePort()
{
    return ntohs(UDP_HEADER_PTR->srcPort);
}

uint16_t UDPSocket::packetDestinationPort()
{
    return ntohs(UDP_HEADER_PTR->destPort);
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
    return ether->getPacket();
}
