#include <stdio.h>

#ifndef HEXT_H
#define HEXT_H

int hext_stream_to_stream(FILE* input, FILE* output);
int hext_filename_to_stream(const char* filename, FILE* output);
int hext_filename_to_buffer(const char* filename, uint8_t *buffer, size_t buffer_len);

#endif
