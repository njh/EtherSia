#ifndef EtherSia_Endian_H
#define EtherSia_Endian_H

/** Convert a 16-bit integer from host (little-endian) to network (big-endian) */
#ifndef htons
#define htons(x) ( (((x)<<8)&0xFF00) | (((x)>>8)&0x00FF) )
#endif

/** Convert a 16-bit integer from network (big-endian) to host (little-endian) */
#ifndef ntohs
#define ntohs(x) htons(x)
#endif

/** Convert a 32-bit integer from host (little-endian) to network (big-endian) */
#ifndef htonl
#define htonl(x) (           \
    (((x)>>24) & 0x000000ff) | \
    (((x)<<8)  & 0x00ff0000) | \
    (((x)>>8)  & 0x0000ff00) | \
    (((x)<<24) & 0xff000000)   \
)
#endif

/** Convert a 32-bit integer from network (big-endian) to host (little-endian) */
#ifndef ntohl
#define ntohl(x) htonl(x)
#endif

/** Convert two bytes to a 16-bit integer */
#define bytesToWord(high, low) \
    (((uint16_t)high << 8) + low);

#endif
