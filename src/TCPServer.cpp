#include "EtherSia.h"
#include "util.h"

TCPServer::TCPServer(EtherSia &ether, uint16_t localPort) : Socket(ether, localPort)
{
}

boolean TCPServer::havePacket()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    if (!_ether.bufferContainsReceived()) {
        return false;
    }

    if (packet.protocol() != IP6_PROTO_TCP) {
        // Wrong protocol
        return false;
    }

    if (packetDestinationPort() != _localPort) {
        // Wrong destination port
        return false;
    }

    if (tcpHeader->flags & TCP_FLAG_RST) {
        return false;
    }

    if (tcpHeader->flags & TCP_FLAG_FIN) {
        tcpHeader->flags = TCP_FLAG_FIN | TCP_FLAG_ACK;
        sendReply((uint16_t)0);
        return false;
    }

    if (tcpHeader->flags & TCP_FLAG_SYN) {
        // Initialise our sequence number to a random number
        // (this is used later in sendInternal)
        tcpHeader->acknowledgementNum = random();

        // Accept the connection
        tcpHeader->flags = TCP_FLAG_SYN | TCP_FLAG_ACK;
        sendReply((uint16_t)0);

        // We have handled the SYN
        return false;
    }

    // Packet contains data that needs to be handled
    if (payloadLength() > 0) {
        _writePos = -1;
        tcpHeader->flags = 0;
        return true;
    }

    // Something else?
    return false;
}

void TCPServer::sendInternal(uint16_t length, boolean /*isReply*/)
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    uint32_t seq = tcpHeader->acknowledgementNum;
    uint32_t ack = ntohl(tcpHeader->sequenceNum);
    uint16_t receivedLen = payloadLength();
    if (receivedLen == 0)
        receivedLen = 1;

    if (tcpHeader->flags == 0) {
        tcpHeader->flags = TCP_FLAG_ACK | TCP_FLAG_FIN | TCP_FLAG_PSH;
    }

    tcpHeader->destinationPort = tcpHeader->sourcePort;
    tcpHeader->sourcePort = htons(_localPort);
    tcpHeader->sequenceNum = seq;
    tcpHeader->acknowledgementNum = htonl(ack + receivedLen);

    tcpHeader->dataOffset = (TCP_TRANSMIT_HEADER_LEN / 4) << 4;
    tcpHeader->window = htons(TCP_WINDOW_SIZE);
    tcpHeader->urgentPointer = 0;

    tcpHeader->mssOptionKind = 2;
    tcpHeader->mssOptionLen = 4;
    tcpHeader->mssOptionValue = tcpHeader->window;

    packet.setPayloadLength(TCP_TRANSMIT_HEADER_LEN + length);

    tcpHeader->checksum = 0;
    tcpHeader->checksum = htons(packet.calculateChecksum());

    _ether.send();
}

uint16_t TCPServer::packetSourcePort()
{
    IPv6Packet& packet = _ether.packet();
    return ntohs(TCP_HEADER_PTR->sourcePort);
}

uint16_t TCPServer::packetDestinationPort()
{
    IPv6Packet& packet = _ether.packet();
    return ntohs(TCP_HEADER_PTR->destinationPort);
}

uint8_t* TCPServer::payload()
{
    IPv6Packet& packet = _ether.packet();
    return packet.payload() + TCP_RECEIVE_HEADER_LEN;
}

uint16_t TCPServer::payloadLength()
{
    IPv6Packet& packet = _ether.packet();
    return packet.payloadLength() - TCP_RECEIVE_HEADER_LEN;
}

uint8_t* TCPServer::transmitPayload()
{
    IPv6Packet& packet = _ether.packet();
    return packet.payload() + TCP_TRANSMIT_HEADER_LEN;
}
