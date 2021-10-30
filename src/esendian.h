#ifndef EtherSia_Endian_H
#define EtherSia_Endian_H

/** Convert a 16-bit integer from host (little-endian) to network (big-endian) */
#ifndef htons
#define htons(x) ( ((x&0x00FFU)<<8) | (((x&0xFF00U)>>8)) )
#endif

/** Convert a 16-bit integer from network (big-endian) to host (little-endian) */
#ifndef ntohs
#define ntohs(x) htons(x)
#endif

/** Convert a 32-bit integer from host (little-endian) to network (big-endian) */
#ifndef htonl
#define htonl(x) (            \
    ((x & 0xff000000U)>>24) | \
    ((x & 0x0000ff00U)<<8)  | \
    ((x & 0x00ff0000U)>>8)  | \
    ((x & 0x000000ffU)<<24)   \
)
#endif

/** Convert a 32-bit integer from network (big-endian) to host (little-endian) */
#ifndef ntohl
#define ntohl(x) htonl(x)
#endif

/** Convert two bytes to a 16-bit integer */
#define bytesToWord(high, low) \
    (((uint16_t)high << 8) + low)

#endif
