#include "util.h"


int8_t asciiToHex(char c)
{
    c |= 0x20;

    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    } else {
        return -1;
    }
}

boolean containsColon(const char *str)
{
    for (const char *ptr = str; *ptr; ptr++) {
        if (*ptr == ':')
            return true;
    }

    return false;
}

void printPaddedHex(uint8_t byte, Print &p)
{
    char str[2];
    str[0] = (byte >> 4) & 0x0f;
    str[1] = byte & 0x0f;

    for (int i=0; i<2; i++) {
        // base for converting single digit numbers to ASCII is 48
        // base for 10-16 to become lower-case characters a-f is 87
        if (str[i] > 9) str[i] += 39;
        str[i] += 48;
        p.print(str[i]);
    }
}

void printPaddedHex16(uint16_t word, Print &p)
{
    printPaddedHex((word & 0xFF00) >> 8, p);
    printPaddedHex((word & 0x00FF) >> 0, p);
}


// This function comes from Contiki's uip6.c
uint16_t chksum(uint16_t sum, const uint8_t *data, uint16_t len)
{
    uint16_t t;
    const uint8_t *dataptr;
    const uint8_t *last_byte;

    dataptr = data;
    last_byte = data + len - 1;

    while(dataptr < last_byte) {   /* At least two more bytes */
        t = (dataptr[0] << 8) + dataptr[1];
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
        dataptr += 2;
    }

    if(dataptr == last_byte) {
        t = (dataptr[0] << 8) + 0;
        sum += t;
        if(sum < t) {
            sum++;      /* carry */
        }
    }

    /* Return sum in host byte order. */
    return sum;
}
