#include <EtherSia.h>
#include "dns.h"

// This function comes from Contiki's resolve.c / encode_name
static uint8_t* encodeName(uint8_t *query, const char *nameptr)
{
    char *nptr;

    --nameptr;
    /* Convert hostname into suitable query format. */
    do {
        uint8_t n = 0;

        ++nameptr;
        nptr = (char *)query;
        ++query;
        for(n = 0; *nameptr != '.' && *nameptr != 0; ++nameptr) {
            *query = *nameptr;
            ++query;
            ++n;
        }
        *nptr = n;
    } while(*nameptr != 0);

    /* End the the name. */
    *query++ = 0;

    return query;
}

static uint16_t makeDNSRequest(uint8_t *buffer, const char *host) {
    struct dnsHeader *dns = (struct dnsHeader*)buffer;
    uint8_t *ptr = buffer;

    dns->id = random(65535);    // FIXME: use sequence instead?
    dns->flags1 = 0x01;         // FIXME: use define flags
    dns->flags2 = 0x00;
    dns->qdcount = htons(1);    // Packet contains a single question
    dns->ancount = htons(0);
    dns->nscount = htons(0);
    dns->arcount = htons(0);
    ptr += sizeof(struct dnsHeader);

    // Name field
    ptr = encodeName(ptr, host);

    // Type and Class fields
    *((uint16_t*)ptr) = htons(DNS_TYPE_AAAA);
    ptr += 2;
    *((uint16_t*)ptr) = htons(DNS_CLASS_IN);
    ptr += 2;

    return ptr-buffer;
}

static uint8_t* skipOverName(uint8_t* ptr)
{
    while (*ptr) {
        // Check if top two bits are set
        if ((*ptr & 0xC0) == 0xC0) {
            // Skip over 'pointer' to somewhere else in packet
            ptr += 2;
            break;
        } else {
            // It is the length of label
            ptr += *ptr + 1;
        }

        // We reached the end of the labels
        if (*ptr == 0) {
            ptr++;
            break;
        }

        // FIXME: check we haven't gone beyond the length of the buffer
    }

    return ptr;
}

static IPv6Address* processDNSReply(uint8_t* payload, uint16_t length)
{
    struct dnsHeader *dns = (struct dnsHeader*)payload;
    uint8_t questionCount = ntohs(dns->qdcount);
    uint8_t answerCount = ntohs(dns->ancount);
    uint8_t *ptr = payload + sizeof(struct dnsHeader);

    // FIXME: check the ID and source
    // FIXME: check it is a reply
    // FIXME: check the flags

    // Skip over the question section
    while(questionCount--) {
        // Name
        ptr = skipOverName(ptr);
        // Type and Class
        ptr += 4;
        // FIXME: check we haven't gone beyond the length of the payload
    }

    while(answerCount--) {
        // Name field
        ptr = skipOverName(ptr);

        // Rest of the record header
        struct dnsRecord* record = (struct dnsRecord*)ptr;

        // Does it look like a valid AAAA record?
        uint16_t rdlength = ntohs(record->rdlength);
        if (record->klass == htons(DNS_CLASS_IN) &&
                record->type == htons(DNS_TYPE_AAAA) &&
                rdlength == sizeof(IPv6Address))
        {
            return (IPv6Address*)(ptr + sizeof(struct dnsRecord));
        }

        // Skip to the next answer
        ptr += sizeof(struct dnsRecord) + rdlength;

        // FIXME: check we haven't gone beyond the length of the payload
    }

    // Failed to find a valid response
    return NULL;
}


IPv6Address* EtherSia::getHostByName(const char* hostname)
{
    unsigned long nextRequest = millis();
    uint8_t requestCount = 0;
    UDPSocket udp(this);

    udp.setRemoteAddress(&dnsServerAddress, DNS_PORT_NUMBER);

    while (requestCount <= DNS_REQUEST_ATTEMPTS) {
        receivePacket();

        if (udp.havePacket()) {
            IPv6Address *address = processDNSReply(udp.payload(), udp.payloadLength());
            if (address) {
                return address;
            }
        }

        if ((long)(millis() - nextRequest) >= 0) {
            uint16_t len = makeDNSRequest(udp.payload(), hostname);
            if (len) {
                udp.send(len);
                nextRequest = millis() + DNS_REQUEST_TIMEOUT;
            }
            requestCount++;
        }
    }

    // Lookup failed
    return NULL;
}
