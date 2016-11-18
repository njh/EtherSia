/*

  hext: a markup language and tool for describing binary files

  Copyright 2016 Nicholas Humfrey

*/

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "hext.h"



static int ascii_to_hex(char c)
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

/* Based on: https://en.wikipedia.org/wiki/Escape_sequences_in_C */
static int escape_to_hex(int c)
{
    switch(c) {
        case '0':  return 0x00;
        case 'a':  return 0x07;
        case 'b':  return 0x08;
        case 'f':  return 0x0C;
        case 'n':  return 0x0A;
        case 'r':  return 0x0D;
        case 't':  return 0x09;
        case 'v':  return 0x0B;
        case '\\': return 0x5C;
        case '\'': return 0x27;
        case '"':  return 0x22;
        case '?':  return 0x3F;
        default:
            return -1;
    }
}

int hext_cb_to_cb(void* input, void* output, hext_read_cb read_cb, hext_write_cb write_cb)
{
    size_t count = 0;

    while (1) {
        int chr = read_cb(input);

        if (chr == EOF) {
            break;

        } else if (isspace(chr) || chr == ':' || chr == '-') {
            /* Ignore */
            continue;

        } else if (chr == '#') {
            /* Ignore the rest of the line */
            while (1) {
                int chr2 = read_cb(input);
                if (chr2 == EOF || chr2 == '\n' || chr2 == '\r')
                    break;
            }

        } else if (isxdigit(chr)) {
            int chr2 = read_cb(input);
            if (!isxdigit(chr2)) {
                fprintf(stderr, "Error: got non-hex digit after hex digit: '%c'\n", chr2);
                break;
            }

            write_cb((ascii_to_hex(chr) << 4) + ascii_to_hex(chr2), output);
            count++;

        } else if (chr == '"') {
            while (1) {
                int chr2 = read_cb(input);
                if (chr2 == EOF || chr2 == '"') {
                    break;
                } else if (chr2 == '\\') {
                    int chr3 = read_cb(input);
                    int escaped = escape_to_hex(chr3);
                    if (escaped < 0) {
                        fprintf(stderr, "Error: invalid escape sequence '%c'\n", chr3);
                        break;
                    } else {
                        write_cb(escaped, output);
                        count++;
                    }
                } else {
                    write_cb(chr2, output);
                    count++;
                }
            }

        } else if (chr == '\\') {
            int chr2 = read_cb(input);
            if (chr2 == EOF) {
                break;
            } else {
                int escaped = escape_to_hex(chr2);
                if (escaped < 0) {
                    fprintf(stderr, "Error: invalid escape sequence '%c'\n", chr2);
                    break;
                } else {
                    write_cb(escaped, output);
                    count++;
                }
            }

        } else {
            fprintf(stderr, "Error: unrecognised character in input: '%c'\n", chr);
            break;
        }
    }

    return count;
}

int hext_stream_to_stream(FILE* input, FILE* output)
{
    /* Explicit cast to avoid compiler warning */
    hext_read_cb read_cb = (hext_read_cb)fgetc;
    hext_write_cb write_cb = (hext_write_cb)fputc;

    return hext_cb_to_cb(input, output, read_cb, write_cb);
}

int hext_filename_to_cb(const char* filename, void* output, hext_write_cb write_cb)
{
    /* Explicit cast to avoid compiler warning */
    hext_read_cb read_cb = (hext_read_cb)fgetc;
    FILE *input = NULL;
    int len = 0;

    input = fopen(filename, "rb");
    if (!input) {
        perror("Failed to open input file");
        return -1;
    }

    len = hext_cb_to_cb(input, output, read_cb, write_cb);

    fclose(input);
    return len;
}

int hext_filename_to_stream(const char* filename, FILE* output)
{
    hext_write_cb write_cb = (hext_write_cb)fputc;

    return hext_filename_to_cb(filename, output, write_cb);
}

struct buffer_write_struct {
    uint8_t *buffer;
    size_t buffer_used;
    size_t buffer_len;
};

static int write_buffer_cb(int c, void* data)
{
    struct buffer_write_struct *bws = (struct buffer_write_struct*)data;

    if (bws->buffer_used + 1 < bws->buffer_len) {
        bws->buffer[bws->buffer_used] = c;
        bws->buffer_used++;
        return c;
    } else {
        /* Buffer isn't big enough */
        return -1;
    }
}

int hext_filename_to_buffer(const char* filename, uint8_t *buffer, size_t buffer_len)
{
    struct buffer_write_struct bws = {NULL, 0, 0};
    int len = 0;

    bws.buffer = buffer;
    bws.buffer_len = buffer_len;

    len = hext_filename_to_cb(filename, &bws, write_buffer_cb);

    return len;
}


#ifdef HEXT_TOOL

enum {
    MODE_BINARY,
    MODE_C,
    MODE_HEXSTREAM
};

static int write_hex(int c, void* output)
{
    int result = fprintf(output, "%2.2x", c);
    if (result < 0) {
        return result;
    } else {
        return c;
    }
}

static void print_c_block(FILE *output, const uint8_t *buffer, int buffer_len)
{
    int i;

    fprintf(output, "uint8_t buffer[] = {");

    for (i=0; i<buffer_len; i++) {
        if (i % 8 == 0) {
            fprintf(output, "\n    ");
        }
        fprintf(output, "0x%2.2x", buffer[i]);
        if (i == buffer_len - 1) {
            fprintf(output, "\n");
        } else {
            fprintf(output, ", ");
        }
    }

    fprintf(output, "};\n");
}

static void usage()
{
    fprintf(stderr, "Usage: hext [options] file...\n");
    fprintf(stderr, "Options\n");
    fprintf(stderr, "  -b  Binary output format (default)\n");
    fprintf(stderr, "  -c  C data structure format\n");
    fprintf(stderr, "  -x  Hexadecimal text stream\n");
    exit(-1);
}

int main(int argc, char **argv)
{
    int opt, mode = 0;

    /* Parse Switches */
    while ((opt = getopt(argc, argv, "bcx")) != -1) {
        switch (opt) {
        case 'b':
            mode = MODE_BINARY;
            break;
        case 'c':
            mode = MODE_C;
            break;
        case 'x':
            mode = MODE_HEXSTREAM;
            break;
        default:
            usage();
            break;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc < 1) {
        fprintf(stderr, "No filename given\n");
        usage();
    }

    if (mode == MODE_BINARY) {
        hext_filename_to_stream(argv[0], stdout);

    } else if (mode == MODE_HEXSTREAM) {
        hext_filename_to_cb(argv[0], stdout, write_hex);

    } else if (mode == MODE_C) {
        uint8_t buffer[4096];
        int len = hext_filename_to_buffer(argv[0], buffer, sizeof(buffer));
        if (len >= 0) {
            print_c_block(stdout, buffer, len);
        }
    }

    return 0;
}

#endif
