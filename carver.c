#include "carver.h"
#include "filetype.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

static size_t global_file_ctr = 0;


static bool store_file(const char *path, const uint8_t *buffer, size_t file_size) {
    FILE *out_file = fopen(path, "wb");
    if (out_file != NULL) {
        size_t written = fwrite(buffer, 1, file_size, out_file);
        fclose(out_file);

        if (written == file_size) {
            printf("Extracted file: %s (%zu Bytes)\n", path, file_size);
            return true;
        }
    } else {
        perror("Failed to store file");
    }
    return false;
}


static ssize_t find_file_end(const uint8_t *buffer, size_t buffer_len,
                             size_t file_offset, const Filetype *type) {
    if (type->footer_len == 0 || file_offset >= buffer_len) {
        return -1;
    }

    // Ensure total file size (including full footer) does not exceed max_size
    size_t max_footer_start = file_offset + type->max_size - type->footer_len + 1;
    size_t search_end = (max_footer_start < buffer_len) ? max_footer_start : buffer_len;

    size_t cur = file_offset;
    while (cur < search_end) {
        const uint8_t *match = memchr(buffer + cur, type->footer[0], search_end - cur);
        if (!match) {
            break;
        }

        cur = match - buffer;
        if (cur + type->footer_len <= buffer_len &&
            memcmp(buffer + cur, type->footer, type->footer_len) == 0) {
            return (ssize_t)(cur + type->footer_len); // Return index where file ends
        }
        cur++;
    }
    return -1; // Not found
}

void *carve_chunk(void *arg) {
    ChunkInfo *chunkInfo = (ChunkInfo *) arg;
    const uint8_t *buffer = chunkInfo->buffer + chunkInfo->chunk_offset;
    size_t buffer_len = chunkInfo->buffer_len - chunkInfo->chunk_offset;
    size_t chunk_size = chunkInfo->chunk_size;

    CarverState state = STATE_SEARCHING;
    Filetype active_type = {0};
    size_t file_offset = 0;
    size_t i = 0;

    while (true) {
        if (state == STATE_SEARCHING) {
            if (i >= chunk_size) {
                break; // We are now out of bounds for this chunk
            }
            bool match_found = false;

            // Check for each filetype if a matching header can be found
            for (size_t filetype_index = 0; filetype_index < NUM_REGISTERED_TYPES; filetype_index++) {
                Filetype type = REGISTERED_TYPES[filetype_index];

                if (i + type.header_len <= buffer_len &&
                    memcmp(buffer + i, type.header, type.header_len) == 0) {

                    active_type = type;
                    file_offset = i;
                    state = STATE_CARVING;
                    i += type.header_len;
                    match_found = true;
                    break;
                }
            }

            if (!match_found) {
                i++;
            }
        } else if (state == STATE_CARVING) {
            ssize_t file_end;
            const char *ext = NULL;
            if (active_type.parse_size == NULL) {
                file_end = find_file_end(buffer, buffer_len, file_offset, &active_type);
            } else {
                ssize_t parsed_len = active_type.parse_size(buffer + file_offset, buffer_len - file_offset, &ext);
                if (parsed_len > 0) {
                    file_end = (ssize_t)file_offset + parsed_len;
                } else {
                    file_end = -1;
                }
            }

            if (file_end >= 0) {
                ext = ext == NULL ? active_type.default_ext : ext;
                size_t current_file_size = (size_t)file_end - file_offset;
                size_t file_id = __atomic_fetch_add(&global_file_ctr, 1, __ATOMIC_RELAXED);

                char filename[256];
                snprintf(filename, sizeof(filename), "%s/extracted_%zu.%s",
                         chunkInfo->output_path, file_id, ext);
                store_file(filename, buffer + file_offset, current_file_size);
            }

            state = STATE_SEARCHING;
            i = file_offset + 1;
        }
    }
    return NULL;
}

void carve(const uint8_t *buffer, size_t buffer_len, int num_threads, const char* output_path) {
    if (num_threads <= 0) {
        num_threads = 1;
    }
    printf("Threads: %d\n", num_threads);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC); // Get the start time

    pthread_t threads[num_threads];
    ChunkInfo chunk_infos[num_threads];

    size_t offset = 0;
    size_t chunk_size = buffer_len / num_threads;

    for (size_t i = 0; i < num_threads; i++) {
        size_t current_chunk_size = (i == num_threads - 1) ? (buffer_len - offset)
                                                           : chunk_size; // The last thread gets the remaining bytes

        chunk_infos[i] = (ChunkInfo) {
                .buffer = buffer,
                .buffer_len = buffer_len,
                .chunk_offset = offset,
                .chunk_size = current_chunk_size,
                .output_path = output_path
        };

        offset += chunk_size;

        if (pthread_create(&threads[i], NULL, carve_chunk, &chunk_infos[i]) != 0) {
            perror("Failed to create thread");
        }
    }

    // Wait for all threads to process their chunk
    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    timespec_get(&end, TIME_UTC);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("Scanning completed in %d seconds.\n", (int) elapsed);
    printf("%zu files found.", global_file_ctr);

}