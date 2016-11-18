/*

  C++ wrapper class to load a hext file into memory

  Copyright 2016 Nicholas Humfrey

*/

#ifndef HEXT_HH
#define HEXT_HH

extern "C" {
    #include "hext.h"
}

class HextFile {
public:
    HextFile(const char* filename) {
        length = hext_filename_to_buffer(filename, buffer, buffer_size);
        if (length <= 0) {
            fprintf(stderr, "Error: failed to load file: %s\n", filename);
        }
    }

    static const int buffer_size = 2048;
    int length = 0;
    uint8_t buffer[buffer_size];
};

#endif
