/**
 * Header file for the MQTT-SN Client
 * @file MQTTSN.h
 */

#ifndef MQTTSN_H
#define MQTTSN_H

#include "UDPSocket.h"


/**
 * Class for connecting to a MQTT-SN server over UDP.
 */
class MQTTSN_Client : public UDPSocket {


public:
    /** Default UDP port number to sent MQTT-SN messages to */
    const uint16_t MQTT_SN_DEFAULT_PORT = 1883;

    /**
     * Construct a MQTT-SN client socket
     *
     * Set the remote server address using setRemoteAddress()
     *
     * @param ether The Ethernet interface to attach the socket to
     */
    MQTTSN_Client(EtherSia &ether);

    /**
     * Set the remote address to connect to
     *
     * @param remoteAddress The remote address or hostname
     * @return true if the remote address was set successfully
     */
    boolean setRemoteAddress(const char *remoteAddress);

    /**
     * Publish a message to the MQTT-SN Server
     *
     * @param topicId The ID of the topic to publish to
     * @param payload A buffer containing the payload/content to publish
     * @param payloadLen The length of the payload
     * @param retain flag for if the the message should be retained
     */
    void publish(const char topic[2], const uint8_t *payload, uint16_t payloadLen, boolean retain=false);

protected:


    /** Enumeration of MQTT-SN packet types */
    enum {
        MQTT_SN_TYPE_PUBLISH = 0x0C
    };

    /** Enumeration of MQTT-SN topic types */
    enum {
        MQTT_SN_TOPIC_TYPE_NORMAL = 0x00,
        MQTT_SN_TOPIC_TYPE_PREDEFINED = 0x01,
        MQTT_SN_TOPIC_TYPE_SHORT = 0x02
    };

    /** Enumeration of MQTT-SN flags */
    enum {
        MQTT_SN_FLAG_RETAIN = (0x1 << 4),
        MQTT_SN_FLAG_QOS_N1 = (0x3 << 5)
    };

};


#endif
