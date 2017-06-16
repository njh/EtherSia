#include "EtherSia.h"
#include "util.h"

MQTTSN_Client::MQTTSN_Client(EtherSia &ether) : UDPSocket(ether)
{
}

boolean MQTTSN_Client::setRemoteAddress(const char *remoteAddress)
{
    return UDPSocket::setRemoteAddress(remoteAddress, MQTT_SN_DEFAULT_PORT);
}

void MQTTSN_Client::publish(const char topic[2], const uint8_t *payload, uint16_t payloadLen, boolean retain)
{
    uint8_t *headerPtr = this->transmitPayload();
    const uint8_t headerLen = 7;

    headerPtr[0] = headerLen + payloadLen;
    headerPtr[1] = MQTT_SN_TYPE_PUBLISH;
    headerPtr[2] = MQTT_SN_FLAG_QOS_N1 | MQTT_SN_TOPIC_TYPE_SHORT;
    if (retain) {
      headerPtr[2] |= MQTT_SN_FLAG_RETAIN;
    }
    headerPtr[3] = topic[0];
    headerPtr[4] = topic[1];
    headerPtr[5] = 0x00;  // Message ID High
    headerPtr[6] = 0x00;  // Message ID Low

    // Copy in the payload
    memcpy(headerPtr + headerLen, payload, payloadLen);
    send(headerPtr[0], false);
}
