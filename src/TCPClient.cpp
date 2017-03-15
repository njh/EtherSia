#include "EtherSia.h"
#include "util.h"

TCPClient::TCPClient(EtherSia &ether) : Socket(ether)
{
    _state = TCP_STATE_DISCONNECTED;
}

void TCPClient::connect()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    // Initialise our sequence number to a random number
    // (this is used later in sendInternal)
    _localSeqNum = random();
    _remoteSeqNum = 0;

    // Use a new local port number for new connections
    _localPort++;

    tcpHeader->flags = TCP_FLAG_SYN;

    _state = TCP_STATE_WAIT_SYN_ACK;

    send((uint16_t)0, false);
}

void TCPClient::disconnect()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;
    _remoteSeqNum += 1;
    
    tcpHeader->flags = TCP_FLAG_FIN | TCP_FLAG_ACK;

    _state = TCP_STATE_FIN;

    send((uint16_t)0, true);
}


boolean TCPClient::connected()
{
    return (_state & 0xF0) != 0x00;
}

void TCPClient::checkForTimeout()
{
}

boolean TCPClient::havePacket()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    if (!_ether.bufferContainsReceived()) {
        // No incoming packets to process - lets check for a timeout
        checkForTimeout();
        return false;
    }

    if (packet.protocol() != IP6_PROTO_TCP) {
        // Wrong protocol
        return false;
    }

    // FIXME: check source/destination IP
    if (packet.destination() != _ether.linkLocalAddress()) {
        // Wrong IP destination - packet is not for us
        return false;
    }
    if (packet.source() != remoteAddress()) {
        // Wrong IP source - packet is not from the expected server
        return false;
    }

    // FIXME: check the sequence numbers are correct
    if (ntohl(tcpHeader->acknowledgementNum) != _localSeqNum)
    {
        //wrong ack number
        return false;
    }
    if (!(tcpHeader->flags & TCP_FLAG_SYN) && (ntohl(tcpHeader->sequenceNum) != _remoteSeqNum))
    {
        //wrong seq number
        return false;
    }
    
    if (ntohs(TCP_HEADER_PTR->destinationPort) != _localPort ||
        ntohs(TCP_HEADER_PTR->sourcePort) != _remotePort)
    {
        // Wrong port pair
        return false;
    }

    if (tcpHeader->flags & TCP_FLAG_RST) {
        Serial.println("RST!");

        _state = TCP_STATE_DISCONNECTED;
        return false;
    }

    if ((tcpHeader->flags & TCP_FLAG_SYN) && (tcpHeader->flags & TCP_FLAG_ACK)) {

        if (_state == TCP_STATE_WAIT_SYN_ACK) {
            // Store the start of the remote sequence number
            // (+1 because SYN is set)
            _remoteSeqNum = ntohl(tcpHeader->sequenceNum) + 1;

            sendAck();

            _state = TCP_STATE_CONNECTED;
        }

        return false;
    } else if (tcpHeader->flags & TCP_FLAG_FIN) {

        if (connected()) {
            // FIN is set on incoming packet
            _remoteSeqNum += 1;

            _state = TCP_STATE_FIN;
        } else {
            _state = TCP_STATE_DISCONNECTED;
        }

    }

    // FIXME: change state to disconnected when our FIN is ACKed
    if ((tcpHeader->flags & TCP_FLAG_FIN) && (tcpHeader->flags & TCP_FLAG_ACK)){
        _state = TCP_STATE_DISCONNECTED;
        sendAck();
    }
    
    // Packet contains data that needs to be handled
    if (payloadLength() > 0) {
        // Got data!
        return true;
    }

    // FIXME: More graceful way than checking this twice?
    if (tcpHeader->flags & TCP_FLAG_FIN) {
        sendAck();
    }

    // Something else?
    return false;
}

void TCPClient::sendAck()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    // FIXME: do we need to store payloadLength() somewhere?

    _remoteSeqNum += payloadLength();

    tcpHeader->flags = TCP_FLAG_ACK;

    if (_state == TCP_STATE_FIN) {
        tcpHeader->flags |= TCP_FLAG_FIN;
    }

    send((uint16_t)0, /*isReply=*/true);
}

void TCPClient::sendInternal(uint16_t length, boolean /*isReply*/)
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    // FIXME: do we ever need to set TCP_FLAG_PSH ?

    packet.setProtocol(IP6_PROTO_TCP);

    tcpHeader->destinationPort = htons(_remotePort);
    tcpHeader->sourcePort = htons(_localPort);
    tcpHeader->sequenceNum = htonl(_localSeqNum);
    tcpHeader->acknowledgementNum = htonl(_remoteSeqNum);

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

    if (tcpHeader->flags & TCP_FLAG_SYN || tcpHeader->flags & TCP_FLAG_FIN) {
        _localSeqNum += 1;
    } else {
        _localSeqNum += length;
    }
}

uint8_t* TCPClient::payload()
{
    IPv6Packet& packet = _ether.packet();
    return packet.payload() + TCP_RECEIVE_HEADER_LEN;
}

uint16_t TCPClient::payloadLength()
{
    IPv6Packet& packet = _ether.packet();
    return packet.payloadLength() - TCP_RECEIVE_HEADER_LEN;
}

uint8_t* TCPClient::transmitPayload()
{
    IPv6Packet& packet = _ether.packet();
    return packet.payload() + TCP_TRANSMIT_HEADER_LEN;
}
