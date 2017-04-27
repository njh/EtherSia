/**
 * Header file for the TCPClient class
 * @file TCPClient.h
 */

#ifndef TCPClient_H
#define TCPClient_H

#include <stdint.h>
#include "IPv6Packet.h"
#include "Socket.h"
#include "tcp.h"

/**
 * Class for open a TCP connection to a client
 *
 */
class TCPClient : public Socket {

public:

    /**
     * Construct a TCP client, with a listening port defined
     *
     * @param ether The Ethernet interface to attach the client to
     */
    TCPClient(EtherSia &ether);

    /**
     * Check if a TCP data packet is available for this client
     *
     * This method also has a side effect of responding to other stages
     * of the TCP sequence (the SYN and FIN packets).
     *
     * After receiving some data, call sendAck() to acknowledge it.
     *
     * @return true if there is a valid packet with data has been received for this client
     */
    boolean havePacket();

    /**
     * Get a pointer to the TCP payload of the last received packet
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    virtual uint8_t* payload();

    /**
     * Get the length (in bytes) of the last received TCP packet payload
     *
     * @note Please call havePacket() first, before calling this method.
     * @return A pointer to the payload
     */
    virtual uint16_t payloadLength();

    /**
     * Get a pointer to the next TCP packet payload to be sent
     *
     * @return A pointer to the transmit payload buffer
     */
    virtual uint8_t* transmitPayload();

    /**
     * Open a new connection to the remote host.
     */
    void connect();
    
    /**
     * Close the existing connection to the remote host.
     */
    void disconnect();

    /**
     * Send an Ack
     */
    void sendAck();

    /**
     * Check if client has successfully connected to the server
     *
     * @return True if the client is connected
     */
    boolean connected();
    
	uint8_t version(){return _version;}

protected:

    enum {
        
        TCP_STATE_DISCONNECTED = 0x00,//0000
        TCP_STATE_WAIT_SYN_ACK = 0x01,//0001
        //we will not use the SYN_RECVD state as client will only do active open
        //TCP_STATE_SYN_RECVD = 0x02,// 0010
        TCP_STATE_CONNECTED = 0x03,//   0011
        TCP_STATE_FIN_WAIT1 = 0x04,//   0100
        TCP_STATE_FIN_WAIT2 = 0x05,//   0101
        TCP_STATE_CLOSING = 0x06,//     0110
        TCP_STATE_TIME_WAIT = 0x07,//   0111
        TCP_STATE_LAST_ACK = 0x08,//    1000

        TCP_STATE_MASK = 0x0f,//1111

        TCP_STATE_STOPPED = 0x10,//10000

        UIP_ACKDATA = 1,//1
        UIP_NEWDATA = 2,//10
        UIP_CLOSE = 16,//10000
        UIP_ABORT = 32,//100000
        UIP_CONNECTED = 64,//1000000
    };
	
	

    /**
     * Protocol specific function that is called by send(), sendReply() etc.
     *
     * @param length The length (in bytes) of the data to send
     * @param isReply Set to true if this packet is a reply to an incoming packet
     */
    virtual void sendInternal(uint16_t length, boolean isReply);
    
    uint8_t _state;
    uint8_t _appliFlags;

    uint16_t _unAckLen;

    uint32_t _remoteSeqNum;
    uint32_t _localSeqNum;

    uint8_t _version=4;

    uint8_t _timer=1;
    uint8_t _sa;
    uint8_t _sv;
    uint8_t _nrtx;
    uint8_t _rto;

};


#endif
