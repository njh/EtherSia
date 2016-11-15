/**
 * Header file for the Syslog class
 * @file Syslog.h
 */

#ifndef Syslog_H
#define Syslog_H

#include "UDPSocket.h"
#include <stdint.h>

/** An enumeration of valid Syslog priority values */
enum SyslogPriority {
    LOG_EMERG = 0,    ///< Emergency: System is unusable
    LOG_ALERT = 1,    ///< Alert: Should be corrected immediately
    LOG_CRIT = 2,     ///< Critical: Critical conditions
    LOG_ERR = 3,      ///< Error: Error conditions
    LOG_WARNING = 4,  ///< Warning: May indicate that an error will occur if action is not taken
    LOG_NOTICE = 5,   ///< Notice: Events that are unusual, but not error conditions
    LOG_INFO = 6,     ///< Informational: Normal operational messages that require no action
    LOG_DEBUG = 7     ///< Debug: Information useful to developers for debugging the application
};

/** An enumeration of valid Syslog facility numbers */
enum SyslogFacility {
    LOG_KERN = 0,     ///< kernel messages
    LOG_USER = 1,     ///< user-level messages
    LOG_MAIL = 2,     ///< mail system
    LOG_DAEMON = 3,   ///< system daemons
    LOG_AUTH = 4,     ///< security/authorization messages

    LOG_LOCAL0 = 16,  ///< no specific purpose, reserved for local use
    LOG_LOCAL1 = 17,  ///< no specific purpose, reserved for local use
    LOG_LOCAL2 = 18,  ///< no specific purpose, reserved for local use
    LOG_LOCAL3 = 19,  ///< no specific purpose, reserved for local use
    LOG_LOCAL4 = 20,  ///< no specific purpose, reserved for local use
    LOG_LOCAL5 = 21,  ///< no specific purpose, reserved for local use
    LOG_LOCAL6 = 22,  ///< no specific purpose, reserved for local use
    LOG_LOCAL7 = 23   ///< no specific purpose, reserved for local use
};



/**
 * Class for sending logging messages to a remote log server over UDP.
 * The Print class is included - so messages can be sent using print() and println(),
 * in the same way as with the Serial object.
 *
 * Log lines will be sent upon writing a newline character (eg using println).
 *
 * @see https://en.wikipedia.org/wiki/Syslog
 */
class Syslog : public UDPSocket {

public:
    /** Default UDP port number to sent Syslog messages to */
    const uint16_t SysLogPortNumber = 514;

    /**
     * Construct a Syslog socket
     *
     * Set the remote server address using setRemoteAddress()
     *
     * @param ether The Ethernet interface to attach the socket to
     */
    Syslog(EtherSia &ether);

    /**
     * Set the remote address to send the syslog packets to
     *
     * @param remoteAddress The remote address or hostname
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(const char *remoteAddress);

    /**
     * Set the priority value for messages
     *
     * See the ::SyslogPriority enumeration for valid values.
     *
     * @param priority The priority value in the range 0-7
     */
    void setPriority(uint8_t priority);

    /**
     * Get the configured priority value for messages
     *
     * See the ::SyslogPriority enumeration for valid values.
     *
     * @return The priority value in the range 0-7
     */
    uint8_t priority();

    /**
     * Set the facility number for messages
     *
     * See the ::SyslogFacility enumeration for valid values.
     *
     * @param facility The facility number in the range 0-25
     */
    void setFacility(uint8_t facility);

    /**
     * Get the configured facility number for messages
     *
     * See the ::SyslogFacility enumeration for valid values.
     *
     * @return The priority value in the range 0-25
     */
    uint8_t facility();

protected:

    boolean handleWriteNewline();

    void writePayloadHeader();

    uint8_t _pri;

};


#endif
