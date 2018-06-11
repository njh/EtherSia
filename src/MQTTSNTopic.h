/**
 * Header file for the MQTT-SN Topic
 * @file MQTTSNTopic.h
 */

#ifndef MQTTSNTopic_H
#define MQTTSNTopic_H


/**
 * Class for storing the details of the MQTT-SN Topic
 */
class MQTTSNTopic {

    public:

    enum {
        TYPE_NORMAL = 0x00,
        TYPE_PREDEFINED = 0x01,
        TYPE_SHORT = 0x02,
        FLAG_FLASH_STRING = 0x40,
    };

    MQTTSNTopic(const char * topicName);
    MQTTSNTopic(const __FlashStringHelper* topicName);
    MQTTSNTopic(uint16_t topicId);
    MQTTSNTopic(char char1, char char2);


    bool isFlashString();

    const char * name() {
        return _name;
    }
    uint16_t id() {
        return _id;
    }
    uint8_t type() {
        return _flags & typeMask;
    }

    protected:

    const uint8_t typeMask = 0x03;

    uint8_t _flags;
    uint16_t _id;
    const char* _name;

    // _isRegistered
    // _isSubscribed
};

#endif
