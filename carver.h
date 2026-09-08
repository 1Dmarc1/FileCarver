#ifndef FILECARVER_CARVER_H
#define FILECARVER_CARVER_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "filetype.h"


typedef enum {
    STATE_SEARCHING,
    STATE_CARVING,
} CarverState;


typedef struct{
    const uint8_t *buffer;
    size_t buffer_len; // The total length of the buffer
    size_t chunk_offset; // The offset for this chunk within the buffer
    size_t chunk_size; // The size of the chunk
    const char* output_path;
} ChunkInfo;


// Scans the provided buffer for files and writes them to the output path.
void carve(const uint8_t *buffer, size_t buffer_len, int num_threads, const char* output_path);

#endif //FILECARVER_CARVER_H
