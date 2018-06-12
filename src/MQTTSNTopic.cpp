#include "EtherSia.h"
#include "util.h"

MQTTSNTopic::MQTTSNTopic(const char * topicName)
{
    _name = topicName;
    _id = 0;
    _flags = TYPE_NORMAL;
}

MQTTSNTopic::MQTTSNTopic(const __FlashStringHelper* topicName)
{
    _name = (const char*)topicName;
    _id = 0;
    _flags = TYPE_NORMAL | flagFlashString;
}

MQTTSNTopic::MQTTSNTopic(const uint16_t topicId)
{
    _name = NULL;
    _id = topicId;
    _flags = TYPE_PREDEFINED;
}

MQTTSNTopic::MQTTSNTopic(const char char1, const char char2)
{
    _name = NULL;
    _id = (char1 << 8) | char2;
    _flags = TYPE_SHORT;
}
