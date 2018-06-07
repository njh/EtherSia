#include "EtherSia.h"
#include "util.h"

#define TFTP_DEBUG(str)
//#define TFTP_DEBUG(str) Serial.println(F(str))

TFTPServer::TFTPServer(EtherSia &ether, uint16_t localPort) : UDPSocket(ether, localPort)
{
}

boolean TFTPServer::handleRequest()
{
    if (!havePacket()) {
        // No packet, or it isn't for us
        return false;
    }

    uint8_t *payload = this->payload();
    if ((payload[0] == 0x00) && (payload[1] == TFTP_OPCODE_READ || payload[1] == TFTP_OPCODE_WRITE)) {
        const char* filename = (char*)(&payload[2]);
        int8_t fileno = openFile(filename);
        if (fileno <= 0) {
            TFTP_DEBUG("TFTP: Error, file not found");
            sendError(TFTP_NOT_FOUND);
            return true;
        }

        if (payload[1] == TFTP_OPCODE_READ) {
            TFTP_DEBUG("TFTP: Starting Read request");
            handleReadRequest(fileno, this->packetSource(), this->packetSourcePort());
        } else if (payload[1] == TFTP_OPCODE_WRITE) {
            TFTP_DEBUG("TFTP: Starting Write request");
            handleWriteRequest(fileno, this->packetSource(), this->packetSourcePort());
        }
    } else {
        TFTP_DEBUG("TFTP: error, illegal operation");
        sendError(TFTP_ILLEGAL_OPERATION);
    }

    return true;
}

void TFTPServer::handleWriteRequest(int8_t fileno, IPv6Address& address, uint16_t port)
{
    UDPSocket data(_ether);
    data.setRemoteAddress(address, port);

    // Acknowledge the request / Start the transfer
    sendAck(data, 0);

    uint32_t timeout = millis() + TFTP_DATA_TIMEOUT;
    uint16_t expectedBlock = 1;
    while (1) {
        _ether.receivePacket();

        if (data.havePacket()) {
            uint8_t *payload = data.payload();

            if (payload[0] == 0x00 && payload[1] == TFTP_OPCODE_DATA) {
                uint16_t len = this->payloadLength() - 4;
                uint16_t block = bytesToWord(payload[2], payload[3]);

                if (block <= expectedBlock) {
                    // Acknowledge the current or past blocks (could be a duplicate packet)
                    // But don't acknowledge future blocks
                    sendAck(data, block);
                } else {
                    TFTP_DEBUG("TFTP: Received out of order block");
                }

                if (block == expectedBlock) {
                    writeBytes(fileno, block, &payload[4], len);
                    expectedBlock++;

                    // Update timeout
                    timeout = millis() + TFTP_DATA_TIMEOUT;

                    // End of transfer?
                    if (len != TFTP_BLOCK_SIZE) {
                        TFTP_DEBUG("TFTP: End of Transfer");
                        break;
                    }
                }
            }
        }

        if ((int32_t)(timeout - millis()) <= 0) {
            TFTP_DEBUG("TFTP: Write Request Timeout");
            return;
        }
    }

}

void TFTPServer::handleReadRequest(int8_t fileno, IPv6Address& address, uint16_t port)
{
    UDPSocket data(_ether);
    data.setRemoteAddress(address, port);

    uint8_t retries = 0;
    for (uint16_t block=1; block<UINT16_MAX;) {
        uint8_t *payload = data.payload();
        payload[0] = 0x00;
        payload[1] = TFTP_OPCODE_DATA;
        payload[2] = (block & 0xFF00) >> 8;
        payload[3] = (block & 0xFF);

        uint16_t len = readBytes(fileno, block, &payload[4]);
        data.send((uint16_t)(len + 4));

        boolean gotAck = waitForAck(data, block);
        if (gotAck) {
            block++;
            retries = 0;
        } else {
            if (++retries > TFTP_RETRIES) {
                // Too many retries, abort
                TFTP_DEBUG("TFTP: abort, too many retries");
                break;
            } else {
                // Try sending again
                TFTP_DEBUG("TFTP: ACK timeout, re-sending packet");
                continue;
            }
        }

        if (len < TFTP_BLOCK_SIZE) {
            // No more data to send
            TFTP_DEBUG("TFTP: finished sending file");
            break;
        }
    }
}

boolean TFTPServer::waitForAck(UDPSocket &sock, uint16_t expectedBlock)
{
    uint32_t timeout = millis() + TFTP_ACK_TIMEOUT;

    do {
        _ether.receivePacket();

        if (sock.havePacket()) {
            uint8_t *payload = sock.payload();
            if (payload[0] == 0x00 && payload[1] == TFTP_OPCODE_ACK) {
                uint16_t recievedBlock = bytesToWord(payload[2], payload[3]);
                if (recievedBlock != expectedBlock) {
                    // Ack for wrong block
                    TFTP_DEBUG("TFTP: Received ack for wrong block");
                    return false;
                } else {
                    // Got correct Ack
                    return true;
                }
            }
        }

    } while ((int32_t)(timeout - millis()) > 0);

    // Timed out, nothing received
    return false;
}

void TFTPServer::sendAck(UDPSocket &sock, uint16_t block)
{
    uint8_t *payload = sock.payload();
    payload[0] = 0x00;
    payload[1] = TFTP_OPCODE_ACK;
    payload[2] = (block & 0xFF00) >> 8;
    payload[3] = (block & 0xFF);
    sock.send((uint16_t)4);
}

void TFTPServer::sendError(uint8_t errorCode)
{
    uint8_t *payload = this->payload();
    uint16_t len = 4;
    payload[0] = 0x00;
    payload[1] = TFTP_OPCODE_ERROR;
    payload[2] = 0x00;
    payload[3] = errorCode;

    const char* errstr;
    switch(errorCode) {
    case TFTP_NOT_FOUND:
        errstr = PSTR("Not Found");
        break;
    case TFTP_ILLEGAL_OPERATION:
        errstr = PSTR("Illegal Op");
        break;
    default:
        errstr = PSTR("Error");
        break;
    }

    len += strlen_P(errstr) + 1;
    strcpy_P((char*)&payload[4], errstr);
    this->sendReply(len);
}
