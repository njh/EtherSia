#include "EtherSia.h"
#include "util.h"

// Uncomment this line to enable debugging
#define MQTTSN_DEBUG(str) Serial.println(F(str))
//#define MQTTSN_DEBUG

MQTTSNClient::MQTTSNClient(EtherSia &ether) : UDPSocket(ether)
{
    setState(MQTT_SN_STATE_DISCONNECTED);
}

boolean MQTTSNClient::setRemoteAddress(const char *remoteAddress)
{
    return UDPSocket::setRemoteAddress(remoteAddress, MQTT_SN_DEFAULT_PORT);
}

void MQTTSNClient::connect()
{
    char clientId[22];
    const MACAddress& mac = _ether.localMac();
    const static char prefix[] PROGMEM = "EtherSia-";

    // Generate clientId from prefix string and MAC Address
    memcpy_P(clientId, prefix, 9);
    for (byte b=0; b < 6; b++) {
        hexToAscii(mac[b], &(clientId[9 + (b*2)]));
    }
    clientId[21] = '\0';
    connect(clientId);
}

void MQTTSNClient::connect(const char* clientId)
{
    setState(MQTT_SN_STATE_CONNECTING);

    _transmitFlags = MQTT_SN_FLAG_CLEAN;
    _transmitBufferLength = strlen(clientId);
    memcpy(_transmitBuffer, clientId, _transmitBufferLength);

    handleConnectFlow();
}

bool MQTTSNClient::checkConnected()
{

    if (_state == MQTT_SN_STATE_PUBLISHING) {
        handlePublishFlow();
    }

    if (_state == MQTT_SN_STATE_CONNECTING) {
        handleConnectFlow();
    }

    // FIXME: perform pings

    return _state & 0xF0 ? true : false;
}


void MQTTSNClient::publish(MQTTSNTopic &topic, const char *payload, int8_t qos, boolean retain)
{
    uint16_t payloadLen = strlen(payload);
    publish(topic, (const uint8_t*)payload, payloadLen, qos, retain);
}

void MQTTSNClient::publish(MQTTSNTopic &topic, const uint8_t *payload, uint16_t payloadLen, int8_t qos, boolean retain)
{
    if (_state != MQTT_SN_STATE_CONNECTED)
        return;

    setState(MQTT_SN_STATE_PUBLISHING);

    _transmitFlags = topic.type();
    if (qos == -1) {
        _transmitFlags |= MQTT_SN_FLAG_QOS_N1;
    } else {
        _transmitFlags |= MQTT_SN_FLAG_QOS_0;
    }
    if (retain) {
        _transmitFlags |= MQTT_SN_FLAG_RETAIN;
    }

    // Copy in the payload into the temporary buffer
    memcpy(_transmitBuffer, payload, payloadLen);
    _transmitBufferLength = payloadLen;
    _transmitTopic = &topic;

    handlePublishFlow();
}

void MQTTSNClient::disconnect()
{
    // FIXME: implement disconnecting
    MQTTSN_DEBUG("MQTTSN: Sending DISCONNECT");
}

void MQTTSNClient::handleConnectFlow()
{
    if (havePacket()) {
        uint8_t *headerPtr = this->payload();

        if (headerPtr[1] == MQTT_SN_TYPE_CONNACK) {
            uint8_t returnCode = headerPtr[2];
            MQTTSN_DEBUG("MQTTSN: received CONNACK");

            if (returnCode == MQTT_SN_ACCEPTED) {
                _state = MQTT_SN_STATE_CONNECTED;
            } else {
                _state = MQTT_SN_STATE_REJECTED;
            }
        }
    }

    // FIXME: simplify this logic
    if (_transmitTime == 0 || millis() - _transmitTime > 5000) {
        sendConnectPacket();
    }

}

void MQTTSNClient::handlePublishFlow()
{
    if (_transmitTopic == NULL)
        return;

    if (havePacket()) {
        uint8_t *headerPtr = this->payload();

        if (headerPtr[1] == MQTT_SN_TYPE_REGACK) {
            MQTTSN_DEBUG("MQTTSN: received REGACK");
            uint16_t topicId = word(headerPtr[2], headerPtr[3]);
            _transmitTopic->setId(topicId);
        }
    }

    if (_transmitTopic->id() == 0) {
        // We need to register the topic first
        // FIXME: simplify this logic
        if (_transmitTime == 0 || millis() - _transmitTime > 5000) {
            sendRegisterPacket();
        }
    } else {
        sendPublishPacket();

        // Switch back to the connected / idle state
        setState(MQTT_SN_STATE_CONNECTED);
    }
}

void MQTTSNClient::sendConnectPacket()
{
    uint8_t *headerPtr = this->transmitPayload();
    const uint8_t headerLen = 6;

    headerPtr[2] = _transmitFlags;
    headerPtr[3] = MQTT_SN_PROTOCOL_ID;
    headerPtr[4] = highByte(MQTT_SN_DEFAULT_KEEP_ALIVE);
    headerPtr[5] = lowByte(MQTT_SN_DEFAULT_KEEP_ALIVE);
    memcpy(headerPtr + headerLen, _transmitBuffer, _transmitBufferLength);

    MQTTSN_DEBUG("MQTTSN: Sending CONNECT");
    sendMQTTSN(MQTT_SN_TYPE_CONNECT, headerLen + _transmitBufferLength);
}

void MQTTSNClient::sendPublishPacket()
{
    uint8_t *headerPtr = this->transmitPayload();
    const uint8_t headerLen = 7;

    uint16_t topicId = _transmitTopic->id();
    headerPtr[2] = _transmitFlags;
    headerPtr[3] = highByte(topicId);
    headerPtr[4] = lowByte(topicId);
    headerPtr[5] = 0x00;  // Message ID High
    headerPtr[6] = 0x00;  // Message ID Low
    memcpy(headerPtr + headerLen, _transmitBuffer, _transmitBufferLength);

    MQTTSN_DEBUG("MQTTSN: Sending PUBLISH");
    sendMQTTSN(MQTT_SN_TYPE_PUBLISH, headerLen + _transmitBufferLength);
}

void MQTTSNClient::sendRegisterPacket()
{
    uint8_t *headerPtr = this->transmitPayload();
    const uint8_t headerLen = 6;
    uint8_t topicNameLen;

    if (_transmitTopic->isFlashString()) {
        topicNameLen = strlen_P(_transmitTopic->name());
        memcpy_P(headerPtr + headerLen, _transmitTopic->name(), topicNameLen);
    } else {
        topicNameLen = strlen(_transmitTopic->name());
        memcpy(headerPtr + headerLen, _transmitTopic->name(), topicNameLen);
    }

    headerPtr[2] = 0x00;  // Topic ID High
    headerPtr[3] = 0x00;  // Topic ID Low
    headerPtr[4] = 0x00;  // FIXME: Message ID High
    headerPtr[5] = 0x01;  // FIXME: Message ID Low

    MQTTSN_DEBUG("MQTTSN: Sending REGISTER");
    sendMQTTSN(MQTT_SN_TYPE_REGISTER, headerLen + topicNameLen);
}

void MQTTSNClient::setState(uint8_t state)
{
    _transmitTopic = NULL;
    _transmitBufferLength = 0;
    _transmitTime = 0;
    _state = state;
}

void MQTTSNClient::sendMQTTSN(uint8_t type, uint8_t length)
{
    uint8_t *headerPtr = this->transmitPayload();
    headerPtr[0] = length;
    headerPtr[1] = type;

    _transmitTime = millis();
    send(length, false);
}
