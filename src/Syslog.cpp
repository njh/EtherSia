#include "EtherSia.h"
#include "util.h"

Syslog::Syslog(EtherSia &ether) : UDPSocket(ether)
{
    _pri = (LOG_LOCAL0 << 3) | LOG_INFO;
}

void Syslog::setPriority(uint8_t priority)
{
    _pri = (_pri & 0xF8) + priority;
}

uint8_t Syslog::priority()
{
    return _pri & 0x07;
}

void Syslog::setFacility(uint8_t facility)
{
    _pri = (_pri & 0x07) + (facility << 3);
}

uint8_t Syslog::facility()
{
    return (_pri & 0xF8) >> 3;
}

boolean Syslog::setRemoteAddress(const char *remoteAddress)
{
    return UDPSocket::setRemoteAddress(remoteAddress, SysLogPortNumber);
}

boolean Syslog::handleWriteNewline()
{
    uint8_t *packetBuffer = payload();

    if (_writePos > 0) {
        packetBuffer[_writePos++] = '\0';
        send((uint16_t)_writePos);
        _writePos = -1;
    }

    // Don't write the line break into the packet
    return false;
}

void Syslog::writePayloadHeader()
{
    print('<');
    print(_pri, DEC);
    print('>');
    print(F("EtherSia: "));
}
