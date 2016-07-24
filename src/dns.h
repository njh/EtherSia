#include <stdint.h>

#include "IPv6Address.h"

/** How often to send DNS request packets */
#define DNS_REQUEST_TIMEOUT    (3500)

/** How many times to send DNS request packets */
#define DNS_REQUEST_ATTEMPTS   (3)


#define DNS_PORT_NUMBER        (53)

#define DNS_FLAG_QUERY         (0)
#define DNS_FLAG_RESPONSE      (1 << 7)

enum dnsType {
    DNS_TYPE_A     = 1,
    DNS_TYPE_CNAME = 5,
    DNS_TYPE_PTR   = 12,
    DNS_TYPE_MX    = 15,
    DNS_TYPE_TXT   = 16,
    DNS_TYPE_AAAA  = 28,
    DNS_TYPE_SRV   = 33,
    DNS_TYPE_ANY   = 255,
};

enum dnsClass {
    DNS_CLASS_IN   = 1,
    DNS_CLASS_ANY  = 255
};


/**
 * Structure for accessing the header of a DNS request/response packet
 * @private
 */
struct dnsHeader {
    uint16_t id;
    uint8_t flags1;
    uint8_t flags2;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((__packed__));

/**
 * Structure for accessing a DNS response record header
 * @private
 */
struct dnsRecord {
    // Name field here
    uint16_t type;
    uint16_t klass;
    uint32_t ttl;
    uint16_t rdlength;
    // Data field here
} __attribute__((__packed__));


/**
 * Write a DNS Request for a hostname into a buffer
 * @private
 */
uint16_t dnsMakeRequest(uint8_t *buffer, const char *hostname, uint16_t requestId);

/**
 * Get the pointer a IPv6 Address from a DNS response
 * Returns NULL if it is not a valid response
 * @private
 */
IPv6Address* dnsProcessReply(uint8_t* payload, uint16_t length, uint16_t requestId);
