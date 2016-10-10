/*
 * Copyright (c) 2016, Nicholas Humfrey
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#if !defined(ARDUINO) && defined(__linux__)

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


#include "LinuxSocket.h"


EtherSia_LinuxSocket::EtherSia_LinuxSocket(const char* ifname)
{
    strncpy(this->ifname, ifname, sizeof(this->ifname)-1);
    ifindex = -1;
    sockfd = -1;
}


boolean
EtherSia_LinuxSocket::begin(const MACAddress &address)
{
    _localMac = address;

    ifindex = if_nametoindex(ifname);
    if (ifindex <= 0) {
        perror("if_nametoindex");
        return false;
    }

    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE_IPV6))) == -1) {
        perror("socket(PF_PACKET)");
        return -1;
    }

    /* Set non-blocking mode */
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    /* Set interface to promiscuous mode */
    struct ifreq ifopts;
    int sockopt = 0;
    strncpy(ifopts.ifr_name, ifname, IFNAMSIZ-1);
    ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
    ifopts.ifr_flags |= IFF_PROMISC;
    ioctl(sockfd, SIOCSIFFLAGS, &ifopts);

    /* Allow the socket to be reused */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
        perror("setsockopt(SO_REUSEADDR)");
        close(sockfd);
        return false;
    }

    /* Bind to device */
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifname, IFNAMSIZ-1) == -1)	{
        perror("setsockopt(SO_BINDTODEVICE)");
        close(sockfd);
        return false;
    }

    return EtherSia::begin();
}

/*---------------------------------------------------------------------------*/

uint16_t
EtherSia_LinuxSocket::sendFrame(const uint8_t *data, uint16_t datalen)
{
    struct sockaddr_ll socket_address;

    /* Index of the network device */
    socket_address.sll_ifindex = ifindex;

    /* Address length*/
    socket_address.sll_halen = ETH_ALEN;

    /* Destination MAC */
    IPv6Packet *packet = (IPv6Packet*)data;
    memcpy(&socket_address.sll_addr, packet->etherDestination(), 6);

    /* Send packet */
    int result = sendto(sockfd, data, datalen, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll));
    if (result <= 0) {
        perror("sendto");
        return 0;
    }

    return result;
}

/*---------------------------------------------------------------------------*/

uint16_t
EtherSia_LinuxSocket::readFrame(uint8_t *buffer, uint16_t bufsize)
{
    int result = read(sockfd, buffer, bufsize);
    if (result <= 0) {
        if (errno != EAGAIN)
            perror("Failed to read");
        return 0;
    }

    return result;
}

void
EtherSia_LinuxSocket::end()
{
    if (sockfd > 0) {
        close(sockfd);
        sockfd = -1;
    }
}

#endif
