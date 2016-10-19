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

#if !defined(ARDUINO)

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "dummy.h"


EtherSia_Dummy::EtherSia_Dummy()
{
    for(size_t i=0; i<bufferSize; i++) {
        _sent[i].time = 0;
        _sent[i].length = 0;
        _sent[i].packet = NULL;

        _recieved[i].time = 0;
        _recieved[i].length = 0;
        _recieved[i].packet = NULL;
    }

    _injectCount = 0;
    _recievedCount = 0;
    _sentCount = 0;
}


boolean
EtherSia_Dummy::begin(const MACAddress &address)
{
    _localMac = address;

    return EtherSia::begin();
}


uint16_t
EtherSia_Dummy::sendFrame(const uint8_t *data, uint16_t len)
{
    _sent[_sentCount].packet = (IPv6Packet *)malloc(len);
    assert(_sent[_sentCount].packet != NULL);

    memcpy(_sent[_sentCount].packet, data, len);
    _sent[_sentCount].length = len;
    _sent[_sentCount].time = time(NULL);

    _sentCount++;

    return 0;
}


uint16_t
EtherSia_Dummy::readFrame(uint8_t *buffer, uint16_t bufsize)
{
    if (_recievedCount < _injectCount) {
        frame_t* frame = &_recieved[_recievedCount++];
        if (frame->length < bufsize) {
            memcpy(buffer, frame->packet, frame->length);
            return frame->length;
        } else {
            // Packet is too big for EtherSia buffer
            return 0;
        }
    } else {
        // Tried to read packet but none available
        return 0;
    }
}


frame_t&
EtherSia_Dummy::getSent(size_t pos)
{
    return _sent[pos];
}


frame_t&
EtherSia_Dummy::getLastSent()
{
    return _sent[_sentCount-1];
}


void
EtherSia_Dummy::end()
{
    clearSent();
    clearRecieved();
}

void EtherSia_Dummy::clearSent()
{
    for(size_t i=0; i<bufferSize; i++) {
        _sent[i].time = 0;
        _sent[i].length = 0;
        if (_sent[i].packet) {
            free(_sent[i].packet);
            _sent[i].packet = NULL;
        }
    }
    _sentCount = 0;
}

void EtherSia_Dummy::clearRecieved()
{
    for(size_t i=0; i<bufferSize; i++) {
        _recieved[i].time = 0;
        _recieved[i].length = 0;
        if (_recieved[i].packet) {
            free(_recieved[i].packet);
            _recieved[i].packet = NULL;
        }
    }
    _injectCount = 0;
    _recievedCount = 0;
}


void
EtherSia_Dummy::injectRecievedPacket(void *packet, uint16_t length)
{
    _recieved[_injectCount].packet = (IPv6Packet *)malloc(length);
    assert(_recieved[_injectCount].packet != NULL);

    memcpy(_recieved[_injectCount].packet, packet, length);
    _recieved[_injectCount].length = length;
    _recieved[_injectCount].time = time(NULL);

    _injectCount++;
}

#endif
