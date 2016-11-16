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

// Based on: https://en.wikipedia.org/wiki/Escape_sequences_in_C
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

int hext_stream_to_stream(FILE* input, FILE* output)
{
    size_t count = 0;

    while (!feof(input) && !feof(output)) {
        int chr = fgetc(input);

        if (chr == EOF) {
            break;

        } else if (isspace(chr) || chr == ':' || chr == '-') {
            // Ignore
            continue;

        } else if (chr == '#') {
            // Ignore the rest of the line
            while (!feof(input)) {
                int chr2 = fgetc(input);
                if (chr2 == '\n' || chr2 == '\r')
                    break;
            }

        } else if (isxdigit(chr)) {
            int chr2 = fgetc(input);
            if (!isxdigit(chr2)) {
                fprintf(stderr, "Error: got non-hex digit after hex digit: '%c'\n", chr2);
                break;
            }

            fputc((ascii_to_hex(chr) << 4) + ascii_to_hex(chr2), output);
            count++;

        } else if (chr == '"') {
            while (!feof(input)) {
                int chr2 = fgetc(input);
                if (chr2 == EOF || chr2 == '"') {
                    break;
                } else if (chr2 == '\\') {
                    int chr3 = fgetc(input);
                    int escaped = escape_to_hex(chr3);
                    if (escaped < 0) {
                        fprintf(stderr, "Error: invalid escape sequence '%c'\n", chr3);
                        break;
                    } else {
                        fputc(escaped, output);
                        count++;
                    }
                } else {
                    fputc(chr2, output);
                    count++;
                }
            }

        } else if (chr == '\\') {
            int chr2 = fgetc(input);
            if (chr2 == EOF) {
                break;
            } else {
                int escaped = escape_to_hex(chr2);
                if (escaped < 0) {
                    fprintf(stderr, "Error: invalid escape sequence '%c'\n", chr2);
                    break;
                } else {
                    fputc(escaped, output);
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

int hext_filename_to_stream(const char* filename, FILE* output)
{
    FILE *input = fopen(filename, "rb");
    if (!input) {
        perror("Failed to open input file");
        return -1;
    }

    int len = hext_stream_to_stream(input, output);

    fclose(input);
    return len;
}

struct buffer_write_struct {
    uint8_t *buffer;
    size_t buffer_used;
    size_t buffer_len;
};

static int write_buffer(void *cookie, const char *ptr, int len)
{
    struct buffer_write_struct *bws = (struct buffer_write_struct*)cookie;
    
    if (bws->buffer_used + len < bws->buffer_len) {
        for(int i=0; i<len; i++) {
            bws->buffer[bws->buffer_used] = ptr[i];
            bws->buffer_used++;
        }
        return len;
    } else {
        // Buffer isn't big enough
        return -1;
    }
}

int hext_filename_to_buffer(const char* filename, uint8_t *buffer, size_t buffer_len)
{
    struct buffer_write_struct bws = {buffer, 0, buffer_len};   
    FILE* output = fwopen(&bws, write_buffer);

    int len = hext_filename_to_stream(filename, output);

    fclose(output);

    return len;
}


#ifdef HEXT_TOOL

enum {
    MODE_BINARY,
    MODE_C,
    MODE_HEXSTREAM
};

static int write_hex(void *cookie, const char *ptr, int len)
{
    FILE* output = (FILE*)cookie;
    size_t count = 0;

    for (int i=0; i<len; i++) {
        int result = fprintf(output, "%2.2x", (const unsigned char)ptr[i]);
        if (result < 0) {
            return result;
        } else {
            count += result;
        }
    }

    return count;
}

static void print_c_block(FILE *output, const uint8_t *buffer, int buffer_len)
{
    fprintf(output, "uint8_t buffer[] = {");

    for (int i=0; i<buffer_len; i++) {
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

    // Parse Switches
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
        FILE* output = fwopen(stdout, write_hex);
        hext_filename_to_stream(argv[0], output);
        fclose(output);

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
