/**
 * Header file for the TFTPServer class
 * @file TFTPServer.h
 */

#ifndef TFTPServer_H
#define TFTPServer_H

#include <stdint.h>
#include "UDPSocket.h"

/**
 * Class for responding to Trivial File Transfer Protocol (TFTP) requests
 *
 * Note that this class must be sub-classed before it can be used.
 *
 * In the subclass, the following methods must be implemented:
 * - openFile()
 * - writeBytes()
 * - readBytes()
 *
 * Warning: after a read or write request is initiated other packets are
 * ignored until the transfer is complete.
 *
 */
class TFTPServer: public UDPSocket {

public:

    /**
     * Construct a TFTP server
     *
     * @param ether The Ethernet interface to attach the TFTP server to
     * @param localPort The local UDP port number to listen on (default is 69)
     */
    TFTPServer(EtherSia &ether, uint16_t localPort=69);

    /**
     * Handle TFTP packets
     *
     * This method must be called to check the packets in loop() after receivePacket()
     *
     * @return Returns true if the incoming packet was a TFTP one
     */
    boolean handleRequest();


    /// The maximum size of payload in a DATA packet
    const uint16_t TFTP_BLOCK_SIZE = 512;

    /// How long to wait for a DATA packet
    /// This has to be high because TFTP clients seem to take a long time to re-send packets
    const uint16_t TFTP_DATA_TIMEOUT = 10000; // 10 seconds

    /// How long to wait for an ACK packet before sending DATA again
    const uint16_t TFTP_ACK_TIMEOUT = 100; // 100 milliseconds

    /// How many times to attempt re-send of data packets that havn't been acknowledged
    const uint8_t TFTP_RETRIES = 10;

protected:

    /**
     * Open/check that a filename is valid
     *
     * @param filename The filename from the TFTP request
     * @return a positive fileno if successful, a negative number if unsuccessful
     */
    virtual int8_t openFile(const char* filename) = 0;

    /**
     * Write bytes received in a TFTP transfer
     *
     * This method is called once for each block received.
     *
     * @param fileno The file number being written to (as returned by openFile)
     * @param block  The TFTP block number (starting at 1)
     * @param data   A pointer to the bytes to be written
     * @param len    The number of bytes in the block
     */
    virtual void writeBytes(int8_t fileno, uint16_t block, const uint8_t* data, uint16_t len) = 0;

    /**
     * Read bytes requested in a TFTP transfer
     *
     * This method is called once for each block to be sent.
     *
     * @param fileno The file number being read from (as returned by openFile)
     * @param block  The TFTP block number (starting at 1)
     * @param data   A pointer to the buffer to copy to read data into
     * @return the length of the current block
     * (if less than TFTP_BLOCK_SIZE, then it is the last block in the transfer)
     */
    virtual int16_t readBytes(int8_t fileno, uint16_t block, uint8_t* data) = 0;



    enum {
        TFTP_OPCODE_READ = 1,
        TFTP_OPCODE_WRITE = 2,
        TFTP_OPCODE_DATA = 3,
        TFTP_OPCODE_ACK = 4,
        TFTP_OPCODE_ERROR = 5
    };

    enum {
        TFTP_UNDEFINED_ERROR = 0,
        TFTP_NOT_FOUND = 1,
        TFTP_ACCESS_VIOLATION = 2,
        TFTP_DISK_FULL = 3,
        TFTP_ILLEGAL_OPERATION = 4,
        TFTP_UNKNOWN_TRANSFER_ID = 5,
        TFTP_FILE_ALREADY_EXISTS = 6,
        TFTP_NO_SUCH_USER = 7,
    };

    void handleWriteRequest(int8_t fileno, IPv6Address& address, uint16_t port);
    void handleReadRequest(int8_t fileno, IPv6Address& address, uint16_t port);

    boolean waitForAck(UDPSocket &sock, uint16_t expectedBlock);
    void sendAck(UDPSocket &sock, uint16_t block);
    void sendError(uint8_t errorCode);

};


#endif
