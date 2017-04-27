#include "EtherSia.h"
#include "util.h"

#define PRINT(...) //Serial.print(__VA_ARGS__)
#define PRINTLN(...) //Serial.println(__VA_ARGS__)

TCPClient::TCPClient(EtherSia &ether) : Socket(ether)
{
    _state = TCP_STATE_DISCONNECTED;
}

//ACTIVE OPEN 
void TCPClient::connect()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;

    // Initialise our sequence number to a random number
    // (this is used later in sendInternal)
    _localSeqNum = random();
    PRINT("randomSN : ");PRINTLN(_localSeqNum);

    _remoteSeqNum = 0;

    // Use a new local port number for new connections
    _localPort++;

    //_unAckLen is the number of unacknowledged sent bytes
    //TCP length of our SYN is 1 byte
    _unAckLen=1;

    //RTT related parameters
    //FIXME not implemented 
    _timer=1;
    _sa=0;
    _sv=16;
    _nrtx=0;
    _rto=3;

    tcpHeader->flags = TCP_FLAG_SYN;

    _state = TCP_STATE_WAIT_SYN_ACK;

    send((uint16_t)0, false);
	
}

void TCPClient::disconnect()
{
    PRINT("Disconnecting...");
    _state |= TCP_STATE_STOPPED;

}


boolean TCPClient::connected()
{
    return ((_state & TCP_STATE_MASK) == TCP_STATE_CONNECTED);
}


boolean TCPClient::havePacket()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;
    static uint16_t tmp16;
    static uint32_t tmp32;
    char* payload = (char*)this->payload();
	
    //in time wait, we wait for 120 pulses and go to disconnected state
    if (_state == TCP_STATE_TIME_WAIT) {
        _timer ++;
        if(_timer==120) {
            _state=TCP_STATE_DISCONNECTED;
            PRINT("[STATE:x");PRINT(_state,HEX);PRINTLN("]");
        }
    }

    //in disconnect mode we do not accept incoming packets, as we only realize active open
    if (_state == TCP_STATE_DISCONNECTED) return false;

    //TO BE FIXED : retransmission algorithm to implement
    if(_unAckLen>0){
    	
    }

    if (!_ether.bufferContainsReceived()) {
        // No incoming packets to process 
        return false;
    }
	
    if (packet.destination() != _ether.linkLocalAddress() || (packet.source() != remoteAddress())) {
        // Wrong IP pair - packet is not for us or not from the good source
        // we drop the packet !!
        return false;
    }
	
    if (packet.protocol() != IP6_PROTO_TCP) {
        // Wrong protocol
        // we drop the packet !!
        return false;
    }
	
    if (ntohs(tcpHeader->sourcePort) != _remotePort) {
        //wrong incoming port
        // we drop the packet !!
        return false;
    }
	
    PRINT("[HL:");PRINT(packet.hopLimit());
    PRINT("--unAckLen:");PRINT(_unAckLen);
    PRINT("--size:");PRINT(payloadLength());
    PRINT("--LPort:");PRINT(_localPort);
    PRINT("--destPortRCV:");PRINT(ntohs(tcpHeader->destinationPort));
    PRINT("--RPort:");PRINT(_remotePort);
    PRINT("--RSN:");PRINT(_remoteSeqNum);
    PRINT("--LSN:");PRINT(_localSeqNum);
    PRINT("--SNRCV:");PRINT(ntohl(tcpHeader->sequenceNum));
    PRINT("--ACKSNRCV:");PRINT(ntohl(tcpHeader->acknowledgementNum));
    PRINT("--flags:");PRINT(tcpHeader->flags,BIN);
    PRINT("--STATE:x");PRINT(_state,HEX);
    PRINT("--appliFlags:");PRINT(_appliFlags);
    PRINTLN("]");

    if (ntohs(tcpHeader->destinationPort) == _localPort) {
            //we've found a packet for us
            PRINTLN("found");
            goto found;
    }
	
    /**
    at this stage, either the packet is a SYN for a new connection, either it is an old packet
    we can't accept a SYN for a new connection as we practise only ACTIVE OPEN, so we drop it
    in case of an old packet, we have to make a reset
    */
    if (tcpHeader->flags & TCP_FLAG_SYN) return false;

    reset :
        //we cannot send resets in response to resets 
        if (tcpHeader->flags & TCP_FLAG_RST) return false; 

        tcpHeader->flags = TCP_FLAG_RST | TCP_FLAG_ACK;
        //prepareReply() swaps ethernet source and destination mac and IP and fixes a fresh hopLimit
        _ether.prepareReply();
        packet.setProtocol(IP6_PROTO_TCP);

        //swap ports
        tmp16 = tcpHeader->sourcePort;
        tcpHeader->sourcePort = tcpHeader->destinationPort;
        tcpHeader->destinationPort = tmp16;

        //swap sequence numbers and add 1 to the sequence number we are acknowledging
        tmp32 = ntohl(tcpHeader->sequenceNum);
        tcpHeader->sequenceNum = tcpHeader->acknowledgementNum;
        tcpHeader->acknowledgementNum = htonl(tmp32+1);

        /**
        we don't send any data nor options in a reset
        just an IP payload consisting in a TCP header of 20 bytes
        */
        goto tcp_send_nodata ;

    found :
        _appliFlags=0;
        if (tcpHeader->flags & TCP_FLAG_RST) {
            _state=TCP_STATE_DISCONNECTED;
            _appliFlags=UIP_ABORT;
            PRINT("[STATE:x");PRINT(_state,HEX);PRINTLN("]");
            return false;
        }

        /** in case the sequence number of the incoming packet is not what we're expecting
        we send an ACK with the correct numbers inside
        */
        if (!(_state == TCP_STATE_WAIT_SYN_ACK && (tcpHeader->flags & (TCP_FLAG_SYN|TCP_FLAG_ACK)))){
            if ((payloadLength()>0) || (tcpHeader->flags & TCP_FLAG_FIN)){
                 if (ntohl(tcpHeader->sequenceNum) != _remoteSeqNum) {
                    PRINT("[Incoming SN is not what we expected]");
                    goto tcp_send_ack;
                }
            }
        }

        /********ACTIVE CLOSING
        we must check if we are in a suitable state for an active closing
        This closing evaluation test must be done when we are in the connected state not in any of the further states
        Active closing process has to be launched after all datasendings 
        it has to be done from the application with the disconnect() method
        The server will not accept our FIN if it has outstanding datas so we need to update _remotSeqNum
        */
        if (_state == (TCP_STATE_STOPPED|TCP_STATE_CONNECTED)) {
            if (_unAckLen==0){
                _unAckLen = 1;
                _state = TCP_STATE_FIN_WAIT1;
                _appliFlags |= UIP_CLOSE;
                _nrtx = 0;
                _remoteSeqNum+=payloadLength();
                if(payloadLength()>0)_appliFlags |= UIP_NEWDATA;
                goto tcp_send_finack;
            }
        }

        /**ACKING TEST
         is the packet acknowledging sent datas ?
        */
        if ((tcpHeader->flags & TCP_FLAG_ACK) && (_unAckLen >0)){
            if (ntohl(tcpHeader->acknowledgementNum) == (_localSeqNum + _unAckLen)){
                _localSeqNum += _unAckLen;
                /**This Ack coming in response to a sent packet constitutes a measurement we can use to update the RTT estimation
                RTT : round trip time
                The following code comes from the Van Jacobson's article on congestion avoidance
                Implementation to finalise with a resending process in top of havePacket()
                */
                if (_nrtx == 0) {
                    signed char m;
                    m = _rto - _timer;
                    m = m - (_sa >> 3);
                    _sa += m;
                    if (m < 0) m = -m;
                    m = m - (_sv >> 2);
                    _sv += m;
                    _rto = (_sa >> 3) + _sv;
                }
                _timer = _rto;
                _unAckLen=0;
                _appliFlags=UIP_ACKDATA;
            }
        }

        /**
        ********* at this stage we will do different things according to connexion state
        */
        if ((_state & TCP_STATE_MASK) == TCP_STATE_WAIT_SYN_ACK){
            if ((_appliFlags & UIP_ACKDATA) && (tcpHeader->flags & (TCP_FLAG_SYN|TCP_FLAG_ACK))){
                //FIXME : we should check the offset and parse the MSS Options if present
                _state = TCP_STATE_CONNECTED;
                _remoteSeqNum = ntohl(tcpHeader->sequenceNum) + 1;
                //in UIP it is UIP_CONNECTED|UIP_NEWDATA ??
                _appliFlags = UIP_CONNECTED;
                goto tcp_send_ack;
            }
            //connexion has failed
            _appliFlags = UIP_ABORT;
            _state = TCP_STATE_DISCONNECTED;
            goto reset;
        }

        if ((_state & TCP_STATE_MASK) == TCP_STATE_CONNECTED){

            /********PASSIVE CLOSING
               we receive a FIN
               we can't accept a FIN with unacknowledged sent datas 
               Otherwise the sequence numbers will be screwed up  
            */
            if(tcpHeader->flags & TCP_FLAG_FIN) {
                if(_unAckLen>0) return false;
                _remoteSeqNum += (payloadLength()+1);
                _appliFlags |= UIP_CLOSE;
                if (payloadLength()>0)_appliFlags |= UIP_NEWDATA;
                _unAckLen=1;
                _state=TCP_STATE_LAST_ACK;
                _nrtx=0;
            tcp_send_finack:
                tcpHeader->flags = TCP_FLAG_FIN | TCP_FLAG_ACK;
                goto tcp_send_nodata_SN_stored_in_socket;
            }

            // Got data!
            if (payloadLength()>0){
                _appliFlags |= UIP_NEWDATA;
                _remoteSeqNum += payloadLength();
                goto tcp_send_ack;
            }

            //We should implement a datasending process here
			
            /********ACTIVE CLOSING could be here also
            */

            return false;
        }

        /** 
        ************From here classic finite state machine
         in TCP_STATE_TIME_WAIT we do _timer=0 as in this state we increase it from 0 till it reaches 120
        */
        if ((_state & TCP_STATE_MASK) == TCP_STATE_LAST_ACK){
            if (_appliFlags & UIP_ACKDATA) {
                _state = TCP_STATE_DISCONNECTED;
                _appliFlags = UIP_CLOSE;
                PRINT("[STATE:x");PRINT(_state,HEX);PRINTLN("]");
                return false;
            }
        }

        if ((_state & TCP_STATE_MASK) == TCP_STATE_FIN_WAIT1){

            if (payloadLength()>0) {
                _appliFlags |= UIP_NEWDATA;
                _remoteSeqNum += payloadLength();
            }

            if (tcpHeader->flags & TCP_FLAG_FIN) {
                if (_appliFlags & UIP_ACKDATA) {
                    _state = TCP_STATE_TIME_WAIT;
                    _timer=0;
                }
                else {
                    _state = TCP_STATE_CLOSING;
                }
                _remoteSeqNum+=1;
                _appliFlags |= UIP_CLOSE;
                tcpHeader->flags = TCP_FLAG_ACK;
                goto tcp_send_nodata_SN_stored_in_socket;
            }
            else if (_appliFlags & UIP_ACKDATA) {
                _state = TCP_STATE_FIN_WAIT2;
                return false;
            }

            if (payloadLength()>0) goto tcp_send_ack;

            return false;
        }

        if ((_state & TCP_STATE_MASK) == TCP_STATE_FIN_WAIT2) {

            if (payloadLength()>0) {
                _appliFlags |= UIP_NEWDATA;
                _remoteSeqNum += payloadLength();
            }

            if (tcpHeader->flags & TCP_FLAG_FIN) {
                _state = TCP_STATE_TIME_WAIT;
                _timer=0;
                _remoteSeqNum+=1;
                _appliFlags |= UIP_CLOSE;
                goto tcp_send_ack;
            }

            if (payloadLength()>0) goto tcp_send_ack;

            return false;
        }

        if ((_state & TCP_STATE_MASK) == TCP_STATE_TIME_WAIT){
            goto tcp_send_ack;
        }

        if ((_state & TCP_STATE_MASK) == TCP_STATE_CLOSING){

            if (_appliFlags & UIP_ACKDATA) {
                _state = TCP_STATE_TIME_WAIT;
                _timer=0;
            }

            return false;
        }

    tcp_send_ack :
        tcpHeader->flags = TCP_FLAG_ACK;
    tcp_send_nodata_SN_stored_in_socket :
        /**Here we can process datas received
        if new datas are coming from the server, the UIP_NEWDATA flag should be set to 1 in the _appliFlags 
        */
        if(_appliFlags & UIP_NEWDATA) {
            PRINT("[Srv answer: ");PRINT(payload);PRINTLN("]");
        }
        //prepareReply() swaps ethernet source and destination mac and IP and fixes a fresh hopLimit
        _ether.prepareReply();
        packet.setProtocol(IP6_PROTO_TCP);
        tcpHeader->destinationPort = htons(_remotePort);
        tcpHeader->sourcePort = htons(_localPort);
        tcpHeader->sequenceNum = htonl(_localSeqNum);
        tcpHeader->acknowledgementNum = htonl(_remoteSeqNum);
    tcp_send_nodata :
        tcpHeader->dataOffset = 5 << 4;
        packet.setPayloadLength(20);
        tcpHeader->urgentPointer = 0;
        tcpHeader->checksum = 0;
        tcpHeader->checksum = htons(packet.calculateChecksum());
    //NOTA label send not used right now, but should be if a datasend process is implemented in the connected state
    send :
        PRINT("[Send");
        if ((tcpHeader->flags & 0x3f) & TCP_FLAG_SYN)PRINT("-SYN");
        if ((tcpHeader->flags & 0x3f) & TCP_FLAG_FIN)PRINT("-FIN");
        if ((tcpHeader->flags & 0x3f) & TCP_FLAG_ACK)PRINT("-ACK");
        if ((tcpHeader->flags & 0x3f) & TCP_FLAG_RST)PRINT("-RST");
        if ((tcpHeader->flags & 0x3f) & TCP_FLAG_PSH)PRINT("-PSH");
        PRINT("--HL:");PRINT(packet.hopLimit());
        PRINT("--unAckLen:");PRINT(_unAckLen);
        PRINT("--size:");PRINT(payloadLength());
        PRINT("--LPort:");PRINT(ntohs(tcpHeader->sourcePort));
        PRINT("--RPort:");PRINT(ntohs(tcpHeader->destinationPort));
        PRINT("--LSN:");PRINT(ntohl(tcpHeader->sequenceNum));
        PRINT("--ACKSN:");PRINT(ntohl(tcpHeader->acknowledgementNum));
        PRINT("--flags:");PRINT(tcpHeader->flags,BIN);
        PRINT("--STATE:x");PRINT(_state,HEX);
        PRINT("--appliFlags:");PRINT(_appliFlags);
        PRINTLN("]");
        _ether.send();

        if (_appliFlags & UIP_NEWDATA) return true;
    return false;
}

//not used rigth now
void TCPClient::sendAck()
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;
    /** to develop if wish to minimise the use of goto in havePacket()
    */
}

void TCPClient::sendInternal(uint16_t length, boolean /*isReply*/)
{
    IPv6Packet& packet = _ether.packet();
    struct tcp_header *tcpHeader = TCP_HEADER_PTR;
	
    PRINT("[sendInternal..writePos:");PRINT(_writePos);
    PRINT("--length:");PRINT(length);
    PRINT("--HL:");PRINT(packet.hopLimit());
    PRINT("--LPort:");PRINT(_localPort);
    PRINT("--RPort:");PRINT(_remotePort);
    PRINT("--LSN:");PRINT(_localSeqNum);
    PRINT("--ACKSN:");PRINT(_remoteSeqNum);
    PRINT("--flags:");PRINT(tcpHeader->flags,BIN);
    PRINT("--STATE:x");PRINT(_state,HEX);
    PRINT("--appliFlags:");PRINT(_appliFlags);
    PRINTLN("]");

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
	_unAckLen+=length;
    
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
