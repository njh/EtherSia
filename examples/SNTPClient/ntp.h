
/** The UDP port number to send NTP packets to */
const uint8_t NTP_PORT = 123;

/** Data structure for an NTP packet, based on RFC4330 */
typedef struct ntpStructure {
    /** Leap Indicator, Version Number and Mode fields */
    uint8_t flags;

    /** Stratum number; distance from a primary reference time source */
    uint8_t stratum;

    /** Max Poll interval (exponent of 2 in seconds) */
    uint8_t poll;

    /** Precision of the system clock (exponent of 2 in seconds) */
    int8_t precision;

    /** Total roundtrip delay to the primary reference source */
    int32_t rootDelay;

    /** Maximum error due to the clock frequency tolerance */
    uint32_t rootDispersion;

    /** Identifier for the upstream reference source */
    uint32_t referenceIdentifer;

    /** The time the server clock was last set or corrected (in seconds)*/
    uint32_t referenceTimestampSeconds;

    /** The time the server clock was last set or corrected (fractions of a second) */
    uint32_t referenceTimestampFraction;

    /** The time the the request departed the client for the server (in seconds) */
    uint32_t originateTimestampSeconds;

    /** The time the the request departed the client for the server (fractions of a second) */
    uint32_t originateTimestampFraction;

    /** The time at which the request arrived at the server or the reply arrived at
        the client (in seconds) */
    uint32_t receiveTimestampSeconds;

    /** The time at which the request arrived at the server or the reply arrived at
        the client (fractions of a second) */
    uint32_t receiveTimestampFraction;

    /** The time at which the request departed the client or the reply departed
        the server (in seconds) */
    uint32_t transmitTimestampSeconds;

    /** The time at which the request departed the client or the reply departed
        the server (fractions of a second) */
    uint32_t transmitTimestampFraction;
} __attribute__((__packed__)) ntpType;

