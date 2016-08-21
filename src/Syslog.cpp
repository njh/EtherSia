#include "EtherSia.h"
#include "util.h"

Syslog::Syslog(EtherSia &ether) : UDPSocket(ether)
{
    _pri = (LOG_LOCAL0 << 3) | LOG_INFO;
    _headerWritten = false;
    _logLen = 0;
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
    UDPSocket::setRemoteAddress(remoteAddress, SYSLOG_PORT_NUMBER);
}

size_t Syslog::write(uint8_t chr)
{
    uint8_t *packetBuffer = payload();

    if (chr == '\n' || chr == '\r') {
        if (_logLen) {
            send(_logLen);
            _headerWritten = false;
            _logLen = 0;
        }
    } else {
        if (!_headerWritten) {
            _headerWritten = true;
            _logLen = 0;
            print('<');
            print(_pri, DEC);
            print('>');
            print(F("EtherSia: "));
        }

        packetBuffer[_logLen++] = chr;
    }
}
