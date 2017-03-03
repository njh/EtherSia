#include "EtherSia.h"
#include "util.h"

TFTPServer::TFTPServer(EtherSia &ether, uint16_t localPort) : UDPSocket(ether, localPort)
{
}

void TFTPServer::handleRequest()
{
    if (!havePacket()) {
        // No packet, or it isn't for us
        return;
    }

    uint8_t *payload = this->payload();
    if ((payload[0] == 0x00) && (payload[1] == TFTP_OPCODE_READ || payload[1] == TFTP_OPCODE_WRITE)) {
        const char* filename = (char*)(&payload[2]);
        int8_t fileno = openFile(filename);
        if (fileno <= 0) {
            sendError(TFTP_NOT_FOUND);
            return;
        }

        if (payload[1] == TFTP_OPCODE_READ) {
            Serial.println("TFTP_OPCODE_READ");
            handleReadRequest(fileno, this->packetSource(), this->packetSourcePort());
        } else if (payload[1] == TFTP_OPCODE_WRITE) {
            Serial.println("TFTP_OPCODE_WRITE");
            handleWriteRequest(fileno, this->packetSource(), this->packetSourcePort());
        }
    } else {
        sendError(TFTP_ILLEGAL_OPERATION);
    }
}

void TFTPServer::handleWriteRequest(int8_t fileno, IPv6Address& address, uint16_t port)
{
    UDPSocket data(_ether);
    data.setRemoteAddress(address, port);

    // Acknowledge/Start the transfer
    uint8_t ack[] = {0x00, TFTP_OPCODE_ACK, 0x00, 0x00};
    data.send(ack, (uint16_t)4);

    // FIXME: add timeout
    while(1) {
        _ether.receivePacket();

        if (data.havePacket()) {
            uint8_t *payload = data.payload();

            if (payload[0] == 0x00 && payload[1] == TFTP_OPCODE_DATA) {
                // FIXME: check the block number is in sequence
                uint16_t len = this->payloadLength() - 4;
                uint16_t block = payload[2]; // FIXME: ntohs(&payload[2]);
                writeBytes(fileno, block, &payload[4], len);

                // Send acknowledgement back
                payload[0] = 0x00;
                payload[1] = TFTP_OPCODE_ACK;
                data.send((uint16_t)4);

                if (len != 512) {
                    Serial.println("End of Transfer");
                    break;
                }
            }
        }
    }
}

void TFTPServer::handleReadRequest(int8_t fileno, IPv6Address& address, uint16_t port)
{
}


void TFTPServer::sendError(uint8_t errorCode)
{
    uint8_t *payload = this->payload();
    uint16_t len = 4;
    payload[0] = 0x00;
    payload[1] = TFTP_OPCODE_ERROR;
    payload[2] = 0x00;
    payload[3] = errorCode;

    char* errstr;
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
    strcpy_P(&payload[4], errstr);
    this->sendReply(len);
}


int8_t TFTPServer::openFile(const char* filename)
{
    if (strcmp(filename, "serial") == 0) {
        return 1;
    } else {
        return -1;
    }
}

void TFTPServer::writeBytes(int8_t fileno, uint16_t block, const uint8_t* data, uint16_t len)
{
    Serial.write(data, len);
}

int8_t TFTPServer::readBytes(int8_t fileno, uint16_t block, const uint8_t* data, uint16_t len)
{
    // FIXME: implement this
    return 0;
}


