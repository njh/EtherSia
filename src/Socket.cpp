#include "EtherSia.h"
#include "util.h"

Socket::Socket(EtherSia &ether) : Socket(ether, (uint16_t)0)
{
}

Socket::Socket(EtherSia &ether, uint16_t localPort) : _ether(ether)
{
    _localPort = localPort;
    _remoteAddress.setZero();
    _remotePort = 0;
    _writePos = -1;
}

boolean Socket::setRemoteAddress(const __FlashStringHelper* remoteAddress, uint16_t remotePort)
{
    char temp[64];

    // Copy out of Flash Memory
    strncpy_P(temp, (const char*)remoteAddress, sizeof(temp)-1);

    return setRemoteAddress(temp, remotePort);
}

boolean Socket::setRemoteAddress(const char *remoteAddress, uint16_t remotePort)
{
    if (containsColon(remoteAddress)) {
        // Parse a human readable IPv6 Address string
        if (!_remoteAddress.fromString(remoteAddress)) {
            return false;
        }
    } else {
        // Lookup a hostname
        IPv6Address *address = _ether.lookupHostname(remoteAddress);
        if (address) {
            _remoteAddress = *address;
        } else {
            // Fail
            return false;
        }
    }

    return setRemoteAddress(_remoteAddress, remotePort);
}

boolean Socket::setRemoteAddress(IPv6Address &remoteAddress, uint16_t remotePort)
{
    _remotePort = remotePort;
    _remoteAddress = remoteAddress;

    if (_localPort == 0) {
        _localPort = random(20000, 30000);
    }

    // Work out the MAC address to use
    if (_ether.inOurSubnet(_remoteAddress)) {
        MACAddress *mac = _ether.discoverNeighbour(_remoteAddress);
        if (mac == NULL) {
            return false;
        } else {
            _remoteMac = *mac;
        }
    } else {
        _remoteMac = _ether.routerMac();
    }

    return true;
}

IPv6Address& Socket::remoteAddress()
{
    return _remoteAddress;
}

uint16_t Socket::remotePort()
{
    return _remotePort;
}

uint16_t Socket::localPort()
{
    return _localPort;
}

IPv6Address& Socket::packetSource()
{
    return _ether.packet().source();
}

IPv6Address& Socket::packetDestination()
{
    return _ether.packet().destination();
}

void Socket::send(boolean isReply)
{
    if (_writePos > 0) {
        send(_writePos, isReply);
        _writePos = -1;
    }
}

void Socket::send(const char *data, boolean isReply)
{
    send((const uint8_t *)data, strlen(data), isReply);
}

void Socket::send(const void *data, uint16_t length, boolean isReply)
{
    uint8_t* payload = this->transmitPayload();

    // FIXME: check it isn't too big
    memcpy(payload, data, length);

    send(length, isReply);
}

void Socket::send(uint16_t length, boolean isReply)
{
    IPv6Packet& packet = _ether.packet();

    if (isReply) {
        _ether.prepareReply();
    } else {
        packet.setDestination(_remoteAddress);
        packet.setEtherDestination(_remoteMac);
        _ether.prepareSend();
    }

    sendInternal(length, isReply);
}

void Socket::sendReply() {
    send(true);
}

void Socket::sendReply(uint16_t length) {
    send(length, true);
}

void Socket::sendReply(const char *data) {
    send(data, true);
}

void Socket::sendReply(const void *data, uint16_t length) {
    send(data, length, true);
}

boolean Socket::payloadEquals(const char *str)
{
    return strncmp((char*)payload(), str, payloadLength()) == 0;
}

uint8_t* Socket::transmitPayload()
{
    // This method can be overloaded
    // by default return pointer to the received payload
    return this->payload();
}

boolean Socket::handleWriteNewline()
{
    return true;
}

void Socket::writePayloadHeader()
{
}

size_t Socket::write(uint8_t chr)
{
    uint8_t *payload = this->transmitPayload();
    boolean doWriteChar = true;

    if (chr == '\n' || chr == '\r') {
        doWriteChar = handleWriteNewline();
    } else if (_writePos == -1) {
        _writePos = 0;
        writePayloadHeader();
    }

    if (doWriteChar) {
        payload[_writePos++] = chr;
        return 1;
    } else {
        return 0;
    }
}
