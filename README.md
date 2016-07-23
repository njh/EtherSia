EtherSia
========

A minimal IPv6 library for an [Arduino] with an [ENC28J60] Ethernet controller.


Features
--------
- Neighbour Discovery Protocol / Stateless Autoconfiguration
- UDP Client and Server
- DNS Client


Design Decisions
----------------
1. Optimised for Arduino - not a general purpose library
2. Should be easy to use - this is what Arduino is all about
3. Should work within the constrains of an [Uno] - 32k ROM and 2k RAM
4. Should follow the [Style Guide] and avoid using complex C features like Pointers and Callbacks
5. Decouple the core from protocols where possible, to allow for program size optimisations


Limitations
-----------
- Ethernet only
- No TCP support
- No DHCPv6
- No Routing or RPL
- No fragmentation support
- A single local router on the network is assumed
- The network prefix length is assumed to be /64

If you need a more fully functional IPv6 stack, then take a look at [Contiki].

EtherSia is an IPv6 only library. If you are looking for an IPv4 library for [ENC28J60],
then take a look at [EtherCard].


Compatibility
-------------

EtherSia should work on any Arduino board with [ENC28J60] based network interface.
There are the results of boards I have tested with:

| Board                           | Tested? | Select Pin (CS) | Hardware MAC Address |
|---------------------------------|---------|-----------------|----------------------|
| [Nanode v5]                     | Working | 8               | UNI/O 11AA02E48      |
| [Nanode RF] / Classic           | -       | 8               | MCP79411             |
| [Nano Shield]                   | Working | 10              | None                 |
| Velleman [KA04]/[VMA04]         | -       | 10              | None                 |
| [Ciseco Ethernet Shield] K016   | -       | 10 / Custom     | None                 |
| [Snootlab Gate 0.5]             | -       | 10              | None                 |


License: [3-clause BSD license]


[Arduino]:                http://www.arduino.cc/
[Uno]:                    http://www.arduino.cc/en/Main/ArduinoBoardUno
[Style Guide]:            http://www.arduino.cc/en/Reference/APIStyleGuide
[Contiki]:                http://www.contiki-os.org/
[ENC28J60]:               http://www.microchip.com/ENC28J60
[EtherCard]:              http://github.com/jcw/ethercard
[3-clause BSD license]:   http://opensource.org/licenses/BSD-3-Clause

[Nanode v5]:              https://wiki.london.hackspace.org.uk/view/Project:Nanode
[Nanode RF]:              http://ichilton.github.com/nanode/rf/build_guide.html
[Nano Shield]:            http://www.tweaking4all.com/hardware/arduino/arduino-enc28j60-ethernet/
[KA04]:                   http://www.vellemanprojects.eu/products/view/?id=412244
[VMA04]:                  http://www.vellemanprojects.eu/products/view/?id=412540
[Ciseco Ethernet Shield]: http://openmicros.org/articles/88-ciseco-product-documentation/178-enc28j60-ethernet-shield-how-to-build
[Snootlab Gate 0.5]:      http://shop.snootlab.com/ethernet/85-gate.html
