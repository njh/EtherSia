#include "util.h"
#include <ctype.h>


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

static void printHexDumpAscii(const char* ascii, uint8_t count, Print &p)
{
    uint8_t i;

    for(i=0; i < (15-count)*3; i++) {
        p.print(' ');
    }

    if (count < 7) {
        p.print(' ');
    }

    p.print(F(" |"));
    for(i=0; i<=count; i++) {
        p.print(ascii[i]);
    }

    p.println('|');
}

void printHexDump(const uint8_t bytes[], uint16_t len, Print &p)
{
    char ascii[16];
    uint8_t mod=0;

    for(uint16_t i=0; i<len; i++) {
        mod = i%16;

        if (mod == 0) {
            printPaddedHex16(i, p);
            p.print(F(":  "));
        }

        printPaddedHex(bytes[i], p);
        p.print(' ');
        if (isprint(bytes[i])) {
            ascii[mod] = bytes[i];
        } else {
            ascii[mod] = '.';
        }

        if (mod == 7) {
            p.print(' ');
        } else if (mod == 15) {
            printHexDumpAscii(ascii, mod, p);
        }
    }

    if (mod != 15) {
        printHexDumpAscii(ascii, mod, p);
    }
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
