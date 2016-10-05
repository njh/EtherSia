/**
 * Header file for direct Ethernet frame access to the W5100 controller
 * @file w5100.h
 */

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

#ifndef ETHERSIA_W5100_H
#define ETHERSIA_W5100_H

#include <SPI.h>

#include "EtherSia.h"

/**
 * Send and receive Ethernet frames directly using a Wiznet W5100 controller.
 */
class EtherSia_W5100 : public EtherSia {

public:
    /**
     * Constructor that uses the default hardware SPI pins
     * @param cs the Arduino Chip Select / Slave Select pin (default 10)
     */
    EtherSia_W5100(int8_t cs=SS);

    // Tell the compiler we want to use begin() from the base class
    using EtherSia::begin;

    /**
     * Initialise the Ethernet controller
     * Must be called before sending or receiving Ethernet frames
     *
     * @param address the local MAC address for the Ethernet interface
     * @return Returns true if setting up the Ethernet interface was successful
     */
    boolean begin(const MACAddress &address);

    /**
     * Shut down the Ethernet controlled
     */
    void end();
  
    /**
     * Send an Ethernet frame
     * @param data a pointer to the data to send
     * @param datalen the length of the data in the packet
     * @return the number of bytes transmitted
     */
    virtual uint16_t sendFrame(const uint8_t *data, uint16_t datalen);

    /**
     * Read an Ethernet frame
     * @param buffer a pointer to a buffer to write the packet to
     * @param bufsize the available space in the buffer
     * @return the length of the received packet
     *         or 0 if no packet was received
     */
    virtual uint16_t readFrame(uint8_t *buffer, uint16_t bufsize);

private:
    const uint16_t TxBufferAddress = 0x4000;  /* Internal Tx buffer address of the iinchip */
    const uint16_t RxBufferAddress = 0x6000;  /* Internal Rx buffer address of the iinchip */
    const uint8_t TxBufferSize = 0x3; /* Buffer size configuration: 0=1kb, 1=2kB, 2=4kB, 3=8kB */
    const uint8_t RxBufferSize = 0x3; /* Buffer size configuration: 0=1kb, 1=2kB, 2=4kB, 3=8kB */
    const uint16_t TxBufferLength = (1 << TxBufferSize) << 10; /* Length of Tx buffer in bytes */
    const uint16_t RxBufferLength = (1 << RxBufferSize) << 10; /* Length of Rx buffer in bytes */
    const uint16_t TxBufferMask = TxBufferLength - 1;
    const uint16_t RxBufferMask = RxBufferLength - 1;

    /**
     * Default function to select chip.
     * @note This function help not to access wrong address. If you do not describe this function or register any functions,
     * null function is called.
     */
    inline void wizchip_cs_select()
    {
        digitalWrite(_cs, LOW);
    }

    /**
     * Default function to deselect chip.
     * @note This function help not to access wrong address. If you do not describe this function or register any functions,
     * null function is called.
     */
    inline void wizchip_cs_deselect()
    {
        digitalWrite(_cs, HIGH);
    }

    /**
     * Reset WIZCHIP by softly.
     */
    void wizchip_sw_reset(void);

    /**
     * Reads a 1 byte value from a register.
     * @param address Register address
     * @return The value of register
     */
    uint8_t wizchip_read(uint16_t address);

    /**
     * Reads a 2 byte value from a register.
     * @param address Register address
     * @return The value of register
     */
    uint16_t wizchip_read_word(uint16_t address);

    /**
     * Writes a 1 byte value to a register.
     * @param address Register address
     * @param wb Write data
     * @return void
     */
    void wizchip_write(uint16_t address, uint8_t wb);

    /**
     * Writes a 2 byte value to a register.
     * @param address Register address
     * @param wb Write data
     * @return void
     */
    void wizchip_write_word(uint16_t address, uint16_t word);

    /**
     * It reads sequence data from registers.
     * @param address Register address
     * @param pBuf Pointer buffer to read data
     * @param len Data length
     */
    void wizchip_read_buf(uint16_t address, uint8_t* pBuf, uint16_t len);

    /**
     * It writes sequence data to registers.
     * @param address Register address
     * @param pBuf Pointer buffer to write data
     * @param len Data length
     */
    void wizchip_write_buf(uint16_t address, const uint8_t* pBuf, uint16_t len);

    /**
     * It copies data to internal TX memory
     *
     * @details This function reads the Tx write pointer register and after that,
     * it copies the <i>wizdata(pointer buffer)</i> of the length of <i>len(variable)</i> bytes to internal TX memory
     * and updates the Tx write pointer register.
     * This function is being called by send() and sendto() function also.
     *
     * @param wizdata Pointer buffer to write data
     * @param len Data length
     * @sa recv_data()
     */
    void wizchip_send_data(const uint8_t *wizdata, uint16_t len);

    /**
     * It copies data to your buffer from internal RX memory
     *
     * @details This function read the Rx read pointer register and after that,
     * it copies the received data from internal RX memory
     * to <i>wizdata(pointer variable)</i> of the length of <i>len(variable)</i> bytes.
     * This function is being called by recv() also.
     *
     * @param wizdata Pointer buffer to read data
     * @param len Data length
     * @sa wiz_send_data()
     */
    void wizchip_recv_data(uint8_t *wizdata, uint16_t len);

    /**
     * It discard the received data in RX memory.
     *
     * @details It discards the data of the length of <i>len(variable)</i> bytes in internal RX memory.
     * @param len Data length
     */
    void wizchip_recv_ignore(uint16_t len);

    /**
     * Get @ref S0_TX_FSR register
     * @return uint16_t. Value of @ref S0_TX_FSR.
     */
    uint16_t getS0_TX_FSR();

    /**
     * Get @ref S0_RX_RSR register
     * @return uint16_t. Value of @ref S0_RX_RSR.
     */
    uint16_t getS0_RX_RSR();


    /** Common registers */
    enum {
        MR = 0x0000,    // Mode Register address (R/W)
        GAR = 0x0001,   // Gateway IP Register address(R/W)
        SUBR = 0x0005,  // Subnet mask Register address(R/W)
        SHAR = 0x0009,  // Source MAC Register address(R/W)
        SIPR = 0x000F,  // Source IP Register address(R/W)
        IR = 0x0015,    // Interrupt Register(R/W)
        IMR = 0x0016,   // Socket Interrupt Mask Register(R/W)
        RTR = 0x0017,   // Timeout register address( 1 is 100us )(R/W)
        RCR = 0x0019,   // Retry count register(R/W)
        RMSR = 0x001A,  // Receive Memory Size
        TMSR = 0x001B,  // Transmit Memory Size
    };

    /** Socket 0 registers */
    enum {
        S0_MR = 0x0400,     ///< Socket 0 Mode register(R/W)
        S0_CR = 0x0401,     ///< Socket 0 command register (R/W)
        S0_IR = 0x0402,     ///< Socket 0 interrupt register (R)
        S0_SR = 0x0403,     ///< Socket 0 status register (R)
        S0_PORT = 0x0404,   ///< Socket 0 source port register (R/W)
        S0_DHAR = 0x0406,   ///< Peer MAC register address (R/W)
        S0_DIPR = 0x040C,   ///< Peer IP register address (R/W)
        S0_DPORT = 0x0410,  ///< Peer port register address (R/W)
        S0_MSSR = 0x0412,   ///< Maximum Segment Size(S0_MSSR0) register address (R/W)
        S0_PROTO = 0x0414,  ///< IP Protocol(PROTO) Register (R/W)
        S0_TOS = 0x0415,    ///< IP Type of Service(TOS) Register (R/W)
        S0_TTL = 0x0416,    ///< IP Time to live(TTL) Register (R/W)
        S0_TX_FSR = 0x0420, ///< Transmit free memory size register (R)
        S0_TX_RD = 0x0422,  ///< Transmit memory read pointer register address (R)
        S0_TX_WR = 0x0424,  ///< Transmit memory write pointer register address (R/W)
        S0_RX_RSR = 0x0426, ///< Received data size register (R)
        S0_RX_RD = 0x0428,  ///< Read point of Receive memory (R/W)
        S0_RX_WR = 0x042A,  ///< Write point of Receive memory (R)
    };

    /** Mode register values */
    enum {
        MR_RST = 0x80,    ///< Reset
        MR_PB = 0x10,     ///< Ping block
        MR_AI = 0x02,     ///< Address Auto-Increment in Indirect Bus Interface
        MR_IND = 0x01,    ///< Indirect Bus Interface mode
    };

    /** Socket 0 Mode Register values @ref S0_MR */
    enum {
        S0_MR_CLOSE = 0x00,  ///< Unused socket
        S0_MR_TCP = 0x01,    ///< TCP
        S0_MR_UDP = 0x02,    ///< UDP
        S0_MR_IPRAW = 0x03,  ///< IP LAYER RAW SOCK
        S0_MR_MACRAW = 0x04, ///< MAC LAYER RAW SOCK
        S0_MR_ND = 0x20,     ///< No Delayed Ack(TCP) flag
        S0_MR_MF = 0x40,     ///< Use MAC filter
        S0_MR_MULTI = 0x80,  ///< support multicating
    };

    /** Socket 0 Command Register values @ref S0_CR */
    enum {
        S0_CR_OPEN = 0x01,      ///< Initialise or open socket
        S0_CR_CLOSE = 0x10,     ///< Close socket
        S0_CR_SEND = 0x20,      ///< Update TX buffer pointer and send data
        S0_CR_SEND_MAC = 0x21,  ///< Send data with MAC address, so without ARP process
        S0_CR_SEND_KEEP = 0x22, ///< Send keep alive message
        S0_CR_RECV = 0x40,      ///< Update RX buffer pointer and receive data
    };

    /** Socket 0 Interrupt register values @ref S0_IR */
    enum {
        S0_IR_CON = 0x01,      ///< CON Interrupt
        S0_IR_DISCON = 0x02,   ///< DISCON Interrupt
        S0_IR_RECV = 0x04,     ///< RECV Interrupt
        S0_IR_TIMEOUT = 0x08,  ///< TIMEOUT Interrupt
        S0_IR_SENDOK = 0x10,   ///< SEND_OK Interrupt
    };

    /** Socket 0 Status register values @ref S0_SR */
    enum {
        SOCK_CLOSED = 0x00,      ///< Closed
        SOCK_INIT = 0x13,        ///< Initiate state
        SOCK_LISTEN = 0x14,      ///< Listen state
        SOCK_SYNSENT = 0x15,     ///< Connection state
        SOCK_SYNRECV = 0x16,     ///< Connection state
        SOCK_ESTABLISHED = 0x17, ///< Success to connect
        SOCK_FIN_WAIT = 0x18,    ///< Closing state
        SOCK_CLOSING = 0x1A,     ///< Closing state
        SOCK_TIME_WAIT = 0x1B,   ///< Closing state
        SOCK_CLOSE_WAIT = 0x1C,  ///< Closing state
        SOCK_LAST_ACK = 0x1D,    ///< Closing state
        SOCK_UDP = 0x22,         ///< UDP socket
        SOCK_IPRAW = 0x32,       ///< IP raw mode socket
        SOCK_MACRAW = 0x42,      ///< MAC raw mode socket
    };

    /**
     * Set Mode Register
     * @param (uint8_t)mr The value to be set.
     * @sa getMR()
     */
    inline void setMR(uint8_t mode) {
        wizchip_write(MR, mode);
    }

    /**
     * Get @ref MR.
     * @return uint8_t. The value of Mode register.
     * @sa setMR()
     */
    inline uint8_t getMR() {
        return wizchip_read(MR);
    }

    /**
     * Set @ref SHAR.
     * @param (uint8_t*)shar Pointer variable to set local MAC address. It should be allocated 6 bytes.
     * @sa getSHAR()
     */
    inline void setSHAR(const uint8_t* macaddr)  {
        wizchip_write_buf(SHAR, macaddr, 6);
    }

    /**
     * Get @ref SHAR.
     * @param (uint8_t*)shar Pointer variable to get local MAC address. It should be allocated 6 bytes.
     * @sa setSHAR()
     */
    inline void getSHAR(uint8_t* macaddr) {
        wizchip_read_buf(SHAR, macaddr, 6);
    }

    /**
     * Get @ref S0_TX_WR register
     * @param (uint16_t)txwr Value to set @ref S0_TX_WR
     * @sa GetS0_TX_WR()
     */
    inline uint16_t getS0_TX_WR() {
        return wizchip_read_word(S0_TX_WR);
    }

    /**
     * Set @ref S0_TX_WR register
     * @param (uint16_t)txwr Value to set @ref S0_TX_WR
     * @sa GetS0_TX_WR()
     */
    inline void setS0_TX_WR(uint16_t txwr) {
        wizchip_write_word(S0_TX_WR, txwr);
    }

    /**
     * Get @ref S0_RX_RD register
     * @regurn uint16_t. Value of @ref S0_RX_RD.
     * @sa setS0_RX_RD()
     */
    inline uint16_t getS0_RX_RD() {
        return wizchip_read_word(S0_RX_RD);
    }

    /**
     * Set @ref S0_RX_RD register
     * @param (uint16_t)rxrd Value to set @ref S0_RX_RD
     * @sa getS0_RX_RD()
     */
    inline void setS0_RX_RD(uint16_t rxrd) {
        wizchip_write_word(S0_RX_RD, rxrd);
    }

    /**
     * Set @ref S0_MR register
     * @param mr Value to set @ref S0_MR
     * @sa getS0_MR()
     */
    inline void setS0_MR(uint8_t mr) {
        wizchip_write(S0_MR, mr);
    }

    /**
     * Get @ref S0_MR register
     * @return Value of @ref S0_MR.
     * @sa setS0_MR()
     */
    inline uint8_t getS0_MR() {
        return wizchip_read(S0_MR);
    }

    /**
     * Set @ref S0_CR register, then wait for the command to execute
     * @param (uint8_t)cr Value to set @ref S0_CR
     * @sa getS0_CR()
     */
    void setS0_CR(uint8_t cr);

    /**
     * Get @ref S0_CR register
     * @return uint8_t. Value of @ref S0_CR.
     * @sa setS0_CR()
     */
    inline uint8_t getS0_CR() {
        return wizchip_read(S0_CR);
    }

    /**
     * Get @ref S0_SR register
     * @return uint8_t. Value of @ref S0_SR.
     */
    inline uint8_t getS0_SR() {
        return wizchip_read(S0_SR);
    }

    /**
     * Get @ref S0_IR register
     * @return uint8_t. Value of @ref S0_IR.
     * @sa setS0_IR()
     */
    inline uint8_t getS0_IR() {
        return wizchip_read(S0_IR);
    }

    /**
     * Set @ref S0_IR register
     * @param (uint8_t)ir Value to set @ref S0_IR
     * @sa getS0_IR()
     */
    inline void setS0_IR(uint8_t ir) {
        wizchip_write(S0_IR, ir);
    }


    int8_t _cs;

};

#endif //_W5100_H_



