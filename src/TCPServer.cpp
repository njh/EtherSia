#include "EtherSia.h"
#include "util.h"

TCPServer::TCPServer(EtherSia &ether, uint16_t localPort) : _ether(ether)
{
    _localPort = localPort;
    _responsePos = -1;
}


uint16_t TCPServer::localPort()
{
    return _localPort;
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
        sendReplyWithFlags(0, TCP_FLAG_FIN | TCP_FLAG_ACK);
        return false;
    }

    if (tcpHeader->flags & TCP_FLAG_SYN) {
        // Initialise our sequence number to a random number
        // (this is used later in sendReplyWithFlags)
        tcpHeader->acknowledgementNum = random();

        // Accept the connection
        sendReplyWithFlags(0, TCP_FLAG_SYN | TCP_FLAG_ACK);

        // We have handled the SYN
        return false;
    }

    // Packet contains data that needs to be handled
    if (payloadLength() > 0) {
        _responsePos = -1;
        return true;
    }

    // Something else?
    return false;
}

size_t TCPServer::write(uint8_t chr)
{
    uint8_t *payload = _ether.packet().payload();

    if (_responsePos == -1) {
        _responsePos = TCP_TRANSMIT_HEADER_LEN;
    }

    payload[_responsePos++] = chr;

    return 1;
}

void TCPServer::sendReply()
{
    if (_responsePos > 0) {
        sendReply(NULL, _responsePos - TCP_TRANSMIT_HEADER_LEN);
    }
}

void TCPServer::sendReply(const char *string)
{
    sendReply((const void*)string, strlen(string));
}

void TCPServer::sendReply(const void* data, uint16_t len)
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    if (data) {
        memcpy((uint8_t*)tcpHeader + TCP_TRANSMIT_HEADER_LEN, data, len);
    }

    // Acknowledge the request packet and send response
    sendReplyWithFlags(len, TCP_FLAG_ACK | TCP_FLAG_FIN | TCP_FLAG_PSH );
}

void TCPServer::sendReplyWithFlags(uint16_t len, uint8_t flags)
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    uint32_t seq = tcpHeader->acknowledgementNum;
    uint32_t ack = ntohl(tcpHeader->sequenceNum);
    uint16_t receivedLen = payloadLength();
    if (receivedLen == 0)
        receivedLen = 1;


    _ether.prepareReply();

    tcpHeader->flags = flags;
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

    packet.setPayloadLength(TCP_TRANSMIT_HEADER_LEN + len);

    tcpHeader->checksum = 0;
    tcpHeader->checksum = htons(packet.calculateChecksum());

    _ether.send();
}

IPv6Address& TCPServer::packetSource()
{
    return _ether.packet().source();
}

IPv6Address& TCPServer::packetDestination()
{
    return _ether.packet().destination();
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

boolean TCPServer::payloadEquals(const char *str)
{
    return strncmp((char*)payload(), str, payloadLength()) == 0;
}
