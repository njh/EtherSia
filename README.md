EtherSia
========

A minimal IPv6 library for an [Arduino] with an [ENC28J60] Ethernet controller.

Features
--------
- Neighbour Discovery Protocol / Stateless Autoconfiguration
- UDP Server


Limitations
-----------
- Ethernet only
- No TCP support
- No DHCPv6
- No Routing or RPL
- No fragmentation support
- A single network interface is assumed
- A single local router on the network is assumed

If you need a more fully functional IPv6 stack, then take a look at [Contiki].

EtherSia is an IPv6 only library. If you are looking for an IPv4 library for ENC28J60,
then take a look at [EtherCard].


License: [3-clause BSD license]


[Arduino]:               http://www.arduino.cc/
[Contiki]:               http://www.contiki-os.org/
[ENC28J60]:              http://www.microchip.com/ENC28J60
[EtherCard]:             http://github.com/jcw/ethercard
[3-clause BSD license]:  http://opensource.org/licenses/BSD-3-Clause
