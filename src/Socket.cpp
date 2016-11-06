#include "EtherSia.h"
#include "util.h"

Socket::Socket(EtherSia &ether) : _ether(ether)
{
    _localPort = random(20000, 30000);
    _remoteAddress.setZero();
    _remotePort = 0;
}

Socket::Socket(EtherSia &ether, uint16_t localPort) : _ether(ether)
{
    _localPort = localPort;
    _remoteAddress.setZero();
    _remotePort = 0;
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
