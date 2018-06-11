/**
 * Header file for the MQTT-SN Client
 * @file MQTTSN.h
 */

#ifndef MQTTSN_H
#define MQTTSN_H

#include "UDPSocket.h"


enum {
    MQTT_SN_QOS_N1 = -1,
    MQTT_SN_QOS_0 = 0
};


/**
 * Class for connecting to a MQTT-SN server over UDP.
 */
class MQTTSN_Client : public UDPSocket {


public:
    /** Default UDP port number to sent MQTT-SN messages to */
    static const uint16_t MQTT_SN_DEFAULT_PORT = 1883;
    static const uint16_t MQTT_SN_DEFAULT_KEEP_ALIVE = 60;
    static const boolean MQTT_SN_DEFAULT_CLEAN_SESSION = true;
    static const uint16_t MQTT_SN_MAX_PACKET_LENGTH = 255;
    static const uint8_t  MQTT_SN_PROTOCOL_ID = 0x01;

    enum {
        MQTT_SN_STATE_DISCONNECTED = 0x00,
        MQTT_SN_STATE_CONNECTED = 0x01,
        MQTT_SN_STATE_REJECTED = 0x02,
        MQTT_SN_TOPIC_LOST_CONNECTION = 0x03
    };

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
     * Check if there is an open connection to te MQTT-SN server
     *
     * This function must be called regularly to keep 
     * the connection to the MQTT-SN server active.
     *
     * @return true if the there is an open connection to the server
     */
    bool checkConnected();

    /**
     * Establish a connection to the MQTT-SN server
     */
    void connect();

    /**
     * Establish a connection to the MQTT-SN server
     *
     * @param clientId The remote address or hostname
     */
    void connect(const char *clientId);

    /**
     * Publish a message to the MQTT-SN Server
     *
     * @param topicId The ID of the topic to publish to
     * @param payload A buffer containing the payload/content to publish
     * @param payloadLen The length of the payload
     * @param qos The QoS level to publish at
     * @param retain flag for if the the message should be retained
     */
    void publish(const char topic[2], const uint8_t *payload, uint16_t payloadLen, int8_t qos=-1, boolean retain=false);

    /**
     * Publish a string to the MQTT-SN Server
     *
     * @param topicId The ID of the topic to publish to
     * @param payload A buffer containing the null-terminated string to publish
     * @param qos The QoS level to publish at
     * @param retain flag for if the the message should be retained
     */
    void publish(const char topic[2], const char *payload, int8_t qos=-1, boolean retain=false);


    void disconnect();

protected:

    uint8_t _state;

    /** Enumeration of MQTT-SN packet types */
    enum {
        MQTT_SN_TYPE_ADVERTISE = 0x00,
        MQTT_SN_TYPE_SEARCHGW = 0x01,
        MQTT_SN_TYPE_GWINFO = 0x02,
        MQTT_SN_TYPE_CONNECT = 0x04,
        MQTT_SN_TYPE_CONNACK = 0x05,
        MQTT_SN_TYPE_WILLTOPICREQ = 0x06,
        MQTT_SN_TYPE_WILLTOPIC = 0x07,
        MQTT_SN_TYPE_WILLMSGREQ = 0x08,
        MQTT_SN_TYPE_WILLMSG = 0x09,
        MQTT_SN_TYPE_REGISTER = 0x0A,
        MQTT_SN_TYPE_REGACK = 0x0B,
        MQTT_SN_TYPE_PUBLISH = 0x0C,
        MQTT_SN_TYPE_PUBACK = 0x0D,
        MQTT_SN_TYPE_PUBCOMP = 0x0E,
        MQTT_SN_TYPE_PUBREC = 0x0F,
        MQTT_SN_TYPE_PUBREL = 0x10,
        MQTT_SN_TYPE_SUBSCRIBE = 0x12,
        MQTT_SN_TYPE_SUBACK = 0x13,
        MQTT_SN_TYPE_UNSUBSCRIBE = 0x14,
        MQTT_SN_TYPE_UNSUBACK = 0x15,
        MQTT_SN_TYPE_PINGREQ = 0x16,
        MQTT_SN_TYPE_PINGRESP = 0x17,
        MQTT_SN_TYPE_DISCONNECT = 0x18,
        MQTT_SN_TYPE_WILLTOPICUPD = 0x1A,
        MQTT_SN_TYPE_WILLTOPICRESP = 0x1B,
        MQTT_SN_TYPE_WILLMSGUPD = 0x1C,
        MQTT_SN_TYPE_WILLMSGRESP = 0x1D,
    };

    /** Enumeration of MQTT-SN return codes */
    enum {
        MQTT_SN_ACCEPTED = 0x00,
        MQTT_SN_REJECTED_CONGESTION = 0x01,
        MQTT_SN_REJECTED_INVALID = 0x02,
        MQTT_SN_REJECTED_NOT_SUPPORTED = 0x03,
    };

    /** Enumeration of MQTT-SN topic types */
    enum {
        MQTT_SN_TOPIC_TYPE_NORMAL = 0x00,
        MQTT_SN_TOPIC_TYPE_PREDEFINED = 0x01,
        MQTT_SN_TOPIC_TYPE_SHORT = 0x02,
    };

    /** Enumeration of MQTT-SN flags */
    enum {
        MQTT_SN_FLAG_DUP = (0x1 << 7),
        MQTT_SN_FLAG_QOS_0 = (0x0 << 5),
        MQTT_SN_FLAG_QOS_1 = (0x1 << 5),
        MQTT_SN_FLAG_QOS_2 = (0x2 << 5),
        MQTT_SN_FLAG_QOS_N1 = (0x3 << 5),
        MQTT_SN_FLAG_RETAIN = (0x1 << 4),
        MQTT_SN_FLAG_WILL = (0x1 << 3),
        MQTT_SN_FLAG_CLEAN = (0x1 << 2),
    };

};


#endif
