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
        TYPE_SHORT = 0x02
    };

    /**
     * Construct a topic object from a normal string
     *
     * @param topicName the name of this topic
     */
    MQTTSNTopic(const char * topicName);

    /**
     * Construct a topic object from a flash string (created using the F() macro)
     *
     * @param topicName the name of this topic
     */
    MQTTSNTopic(const __FlashStringHelper* topicName);

    /**
     * Construct a topic object from a pre-defined topic id
     *
     * @param topicId the 16-bit ID of the topic
     */
    MQTTSNTopic(uint16_t topicId);

    /**
     * Construct a topic object from a short (2 character) topic name
     *
     * @param char1 the first character of the topic
     * @param char2 the second character of the topic
     */
    MQTTSNTopic(char char1, char char2);

    /**
     * Check if the topic name is stored in Flash memory
     *
     * @return true if the string is stored in flash memory
     */
    bool isFlashString() {
        return _flags & flagFlashString;
    }

    /**
     * Get the name of the topic
     *
     * @return the topic name or NULL if it doesn't have a name
     */
    const char * name() {
        return _name;
    }

    /**
     * Get the ID of the topic
     *
     * @return the topic id or 0 if it hasn't been registered yet
     */
    uint16_t id() {
        return _id;
    }

    /**
     * Set the ID of the topic
     *
     * @param the id number for this topic
     */
    void setId(uint16_t id) {
        _id = id;
    }

    /**
     * Get the topic type
     *
     * Normal: 0
     * Pre-defined: 1
     * Short: 2
     *
     * @return the topic type
     */
    uint8_t type() {
        return _flags & typeMask;
    }

protected:

    const uint8_t typeMask = 0x03;
    const uint8_t flagFlashString = 0x10;

    uint8_t _flags;
    uint16_t _id;
    const char* _name;

    // _isRegistered
    // _isSubscribed
};

#endif
