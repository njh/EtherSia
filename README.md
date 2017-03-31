EtherSia
========

A minimal IPv6 library for an [Arduino] with an [ENC28J60], [W5500] or [W5100] Ethernet controller.


Features
--------
- SLAAC (Neighbour Discovery Protocol / Stateless Auto-configuration)
- HTTP Server
- UDP Client and Server
- DNS Client


Design Decisions
----------------
1. Optimised for Arduino - not a general purpose library
2. Should be easy to use - this is what Arduino is all about
3. Should work within the constraints of an [Uno] - 32k ROM and 2k RAM
4. Should follow the [Style Guide] and avoid using complex C features like Pointers and Callbacks
5. Decouple the core from protocols where possible, to allow for program size optimisations
6. Only use statically allocated memory to avoid leaks and keep memory usage down


Limitations
-----------
- Ethernet only
- No DHCPv6
- No Routing or RPL
- Stateless TCP (single packet request/response)
- No fragmentation support
- A single local router on the network is assumed
- The network prefix length is assumed to be /64

If you need a more fully functional IPv6 stack, then take a look at [Contiki].

EtherSia is an IPv6 only library. If you are looking for an IPv4 library for [ENC28J60],
then take a look at [EtherCard].


Compatibility
-------------

EtherSia should work on any Arduino board with [ENC28J60], [W5500] or [W5100] based network interface.
There are the results of boards I have tested with:

| Board                         | Class                  | Tested? | CS Pin | Hardware MAC Address |
|-------------------------------|------------------------|---------|--------|----------------------|
| [Arduino Ethernet Shield]     | [EtherSia_W5100]       | Working | 10     | None                 |
| [Arduino Ethernet Shield 2]   | [EtherSia_W5500]       | Working | 10     | None                 |
| [Nanode v5]                   | [EtherSia_ENC28J60]    | Working | 8      | UNI/O 11AA02E48      |
| [Nanode v5]                   | [EtherSia_ENC28J60]    | Working | 8      | UNI/O 11AA02E48      |
| [Nanode RF] / Classic         | [EtherSia_ENC28J60]    | -       | 8      | MCP79411             |
| [Nano Shield]                 | [EtherSia_ENC28J60]    | Working | 10     | None                 |
| Velleman [KA04]/[VMA04]       | [EtherSia_ENC28J60]    | -       | 10     | None                 |
| [Ciseco Ethernet Shield] K016 | [EtherSia_ENC28J60]    | -       | 10     | None                 |
| [Snootlab Gate 0.5]           | [EtherSia_ENC28J60]    | -       | 10     | None                 |
| _Testing on Linux_            | [EtherSia_LinuxSocket] | Working | -      | -                    |

License: [3-clause BSD license]


[Arduino]:                 http://www.arduino.cc/
[Uno]:                     http://www.arduino.cc/en/Main/ArduinoBoardUno
[Style Guide]:             http://www.arduino.cc/en/Reference/APIStyleGuide
[Contiki]:                 http://www.contiki-os.org/
[ENC28J60]:                http://www.microchip.com/ENC28J60
[W5500]:                   http://www.wiznet.io/product-item/w5500/
[W5100]:                   http://www.wiznet.io/product-item/w5100/
[EtherCard]:               http://github.com/jcw/ethercard
[3-clause BSD license]:    http://opensource.org/licenses/BSD-3-Clause

[EtherSia_ENC28J60]:       http://www.aelius.com/njh/ethersia/class_ether_sia___e_n_c28_j60.html
[EtherSia_LinuxSocket]:    http://www.aelius.com/njh/ethersia/class_ether_sia___linux_socket.html
[EtherSia_W5100]:          http://www.aelius.com/njh/ethersia/class_ether_sia___w5100.html
[EtherSia_W5500]:          http://www.aelius.com/njh/ethersia/class_ether_sia___w5500.html

[Arduino Ethernet Shield]:   https://www.arduino.cc/en/Main/ArduinoEthernetShield
[Arduino Ethernet Shield 2]: http://www.arduino.org/products/shields/arduino-ethernet-shield-2
[Nanode v5]:                 https://wiki.london.hackspace.org.uk/view/Project:Nanode
[Nanode RF]:                 http://ichilton.github.com/nanode/rf/build_guide.html
[Nano Shield]:               http://www.tweaking4all.com/hardware/arduino/arduino-enc28j60-ethernet/
[KA04]:                      http://www.vellemanprojects.eu/products/view/?id=412244
[VMA04]:                     http://www.vellemanprojects.eu/products/view/?id=412540
[Ciseco Ethernet Shield]:    http://openmicros.org/articles/88-ciseco-product-documentation/178-enc28j60-ethernet-shield-how-to-build
[Snootlab Gate 0.5]:         http://shop.snootlab.com/ethernet/85-gate.html
