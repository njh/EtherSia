/*
 * Copyright (c) 2013, WIZnet Co., Ltd.
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

#include "w5100.h"


void EtherSia_W5100::wizchip_sw_reset()
{
    setMR(MR_RST);
    getMR(); // for delay

    setSHAR(_localMac);
}


void EtherSia_W5100::wizchip_write(uint16_t AddrSel, uint8_t wb )
{
    wizchip_cs_select();
    SPI.transfer(0xF0);
    SPI.transfer((AddrSel & 0xFF00) >>  8);
    SPI.transfer((AddrSel & 0x00FF) >>  0);
    SPI.transfer(wb);    // Data write (write 1byte data)
    wizchip_cs_deselect();
}

void EtherSia_W5100::wizchip_write_word(uint16_t AddrSel, uint16_t word)
{
    wizchip_write(AddrSel,   (uint8_t)(word>>8));
    wizchip_write(AddrSel+1, (uint8_t) word);
}

uint8_t EtherSia_W5100::wizchip_read(uint16_t AddrSel)
{
    uint8_t ret;

    wizchip_cs_select();
    SPI.transfer(0x0F);
    SPI.transfer((AddrSel & 0xFF00) >>  8);
    SPI.transfer((AddrSel & 0x00FF) >>  0);
    ret = SPI.transfer(0);
    wizchip_cs_deselect();

    return ret;
}

uint16_t EtherSia_W5100::wizchip_read_word(uint16_t AddrSel)
{
    return ((uint16_t)wizchip_read(AddrSel) << 8) + wizchip_read(AddrSel + 1);
}


void EtherSia_W5100::wizchip_write_buf(uint16_t AddrSel, const uint8_t* pBuf, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        wizchip_write(AddrSel + i, pBuf[i]);
    }
}


void EtherSia_W5100::wizchip_read_buf(uint16_t AddrSel, uint8_t* pBuf, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        pBuf[i] = wizchip_read(AddrSel + i);
    }
}

void EtherSia_W5100::setS0_CR(uint8_t cr) {
    // Write the command to the Command Register
    wizchip_write(S0_CR, cr);

    // Now wait for the command to complete
    while( wizchip_read(S0_CR) );
}

uint16_t EtherSia_W5100::getS0_TX_FSR()
{
    uint16_t val=0,val1=0;
    do
    {
        val1 = wizchip_read(S0_TX_FSR);
        val1 = (val1 << 8) + wizchip_read(S0_TX_FSR + 1);
        if (val1 != 0)
        {
            val = wizchip_read(S0_TX_FSR);
            val = (val << 8) + wizchip_read(S0_TX_FSR + 1);
        }
    } while (val != val1);
    return val;
}


uint16_t EtherSia_W5100::getS0_RX_RSR()
{
    uint16_t val=0,val1=0;
    do
    {
        val1 = wizchip_read(S0_RX_RSR);
        val1 = (val1 << 8) + wizchip_read(S0_RX_RSR + 1);
        if (val1 != 0)
        {
            val = wizchip_read(S0_RX_RSR);
            val = (val << 8) + wizchip_read(S0_RX_RSR + 1);
        }
    } while (val != val1);
    return val;
}

void EtherSia_W5100::wizchip_send_data(const uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr;
    uint16_t size;
    uint16_t dst_mask;
    uint16_t dst_ptr;

    ptr = getS0_TX_WR();

    dst_mask = ptr & getS0_TxMASK();
    dst_ptr = TxBufferAddress + dst_mask;

    if (dst_mask + len > getS0_TxMAX())
    {
        size = getS0_TxMAX() - dst_mask;
        wizchip_write_buf(dst_ptr, wizdata, size);
        wizdata += size;
        size = len - size;
        dst_ptr = TxBufferAddress;
        wizchip_write_buf(dst_ptr, wizdata, size);
    }
    else
    {
        wizchip_write_buf(dst_ptr, wizdata, len);
    }

    ptr += len;

    setS0_TX_WR(ptr);
}

void EtherSia_W5100::wizchip_recv_data(uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr;
    uint16_t size;
    uint16_t src_mask;
    uint16_t src_ptr;

    ptr = getS0_RX_RD();

    src_mask = ptr & getS0_RxMASK();
    src_ptr = RxBufferAddress + src_mask;


    if( (src_mask + len) > getS0_RxMAX() )
    {
        size = getS0_RxMAX() - src_mask;
        wizchip_read_buf(src_ptr, wizdata, size);
        wizdata += size;
        size = len - size;
        src_ptr = RxBufferAddress;
        wizchip_read_buf(src_ptr, wizdata, size);
    }
    else
    {
        wizchip_read_buf(src_ptr, wizdata, len);
    }

    ptr += len;

    setS0_RX_RD(ptr);
}


EtherSia_W5100::EtherSia_W5100(int8_t cs)
{
    _cs = cs;
}

boolean EtherSia_W5100::begin(const MACAddress &address)
{
    this->_localMac = address;

    pinMode(_cs, OUTPUT);
    wizchip_cs_deselect();

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    wizchip_sw_reset();

    setSHAR(_localMac);

    setS0_MR(S0_MR_MACRAW);
    setS0_CR(S0_CR_OPEN);

    if (getS0_SR() != SOCK_MACRAW) {
        Serial.println("Failed to put socket 0 into MACRaw mode");
        return false;
    }

    return EtherSia::begin();
}

void EtherSia_W5100::end()
{
    setS0_CR(S0_CR_CLOSE);

    // clear all interrupt of the socket
    setS0_IR(0xFF);

    // Wait for socket to change to closed
    while(getS0_SR() != SOCK_CLOSED);
}

uint16_t EtherSia_W5100::readFrame(uint8_t *buffer, uint16_t bufsize)
{
    uint16_t len = getS0_RX_RSR();
    if ( len > 0 )
    {
        uint8_t head[2];
        uint16_t data_len=0;

        wizchip_recv_data(head, 2);
        setS0_CR(S0_CR_RECV);

        data_len = head[0];
        data_len = (data_len<<8) + head[1];
        data_len -= 2;

        if(data_len > bufsize)
        {
            //Serial.println("Packet is bigger than buffer");
            return 0;
        }

        wizchip_recv_data(buffer, data_len );
        setS0_CR(S0_CR_RECV);

        return data_len;
    }

    return 0;
}

uint16_t EtherSia_W5100::sendFrame(const uint8_t *buf, uint16_t len)
{
    uint16_t freesize = 0;

    // check size not to exceed MAX size.
    freesize = getS0_TxMAX();
    if (len > freesize) len = freesize;

    // Wait for space in the transmit buffer
    while(1)
    {
        freesize = getS0_TX_FSR();
        if(getS0_SR() == SOCK_CLOSED) {
            Serial.println("Socket closed");
            return -1;
        }
        if(len <= freesize) break;
    };

    wizchip_send_data(buf, len);
    setS0_CR(S0_CR_SEND);

    while(1)
    {
        uint8_t tmp = getS0_IR();
        if(tmp & S0_IR_SENDOK)
        {
            setS0_IR(S0_IR_SENDOK);
            Serial.println("S0_IR_SENDOK");
            break;
        }
        else if(tmp & S0_IR_TIMEOUT)
        {
            setS0_IR(S0_IR_TIMEOUT);
            Serial.println("Timeout");
            return -1;
        }
    }

    return len;
}
