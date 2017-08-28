Release History
===============

Version 2.4.0 (2017-08-28)
--------------------------
- Added PingClient class and example
- Added EtherSia::rejectPacket()
- Added MACAddress::operator==
- Added MACAddress::isIPv6Multicast()
- Added checks for Ethernet source and destination address
- Added support for constructing IPv6Address and MACAddress from a flash string
- Switched to using SPISettings class to configure SPI
- Set Traffic Class and Flow Label to 0 when sending replies
- Changed TFTPServer::handleRequest() to return boolean
- Use link local address when sending, if there is no global IP
- Bug fix for Neighbour Discovery for link local address
- Bug fix for neighbour discovery setting wrong source address


Version 2.3.0 (2017-06-04)
--------------------------
- Added 'Server: EtherSia' to HTTP responses
- Added support for passing a flash string to setRemoteAddress()
- MACAddress: added new constructor from 6-byte array


Version 2.2.0 (2017-03-09)
--------------------------
- Added TFTP server
- Added Socket::transmitPayload()
- Added IPv6Packet::etherType()
- Made the Socket class a sub-class of Print
- Added support for sending UDP packets to machines on the same subnet
- Added example of manually configuring a static address
- Added support for manually setting the local router address
- Added EtherSia::disableAutoconfiguration()
- Added EtherSia::routerMac()
- Added EtherSia::inOurSubnet()
- Added EtherSia::discoverNeighbour()
- Created WebToggler example
- Fix for not setting source MAC for ICMP6 NS and RS packets
- Added EtherSia_Dummy driver class for testing EtherSia
- Added HTTPServer::redirect()
- Added methods for inspecting the body of an HTTP request
- Lots of tests added


Version 2.1.0 (2016-10-08)
--------------------------
- Added support for W5500 chip
- Added TCPServer and HTTPServer classes


Version 2.0.0 (2016-09-11)
--------------------------
- Added support for the W5100 Ethernet Controller in MACRaw mode
- Added Syslog class and example
- Refactored ENC28J60 class to be the child of EtherSia


Version 1.0.0 (2016-07-31)
--------------------------
- Initial Release
- Neighbour Discovery Protocol / Stateless Autoconfiguration
- UDP Client and Server
- DNS Client
