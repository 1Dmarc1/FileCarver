#ifndef FILECARVER_FILETYPE_H
#define FILECARVER_FILETYPE_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// Function pointer for dynamic calculating the size of a file if it doesn't have a footer.
// Returns total byte length, or 0 if false positive.
typedef ssize_t (*SizeParserFunc)(const uint8_t *buf, size_t remaining_bytes, const char **out_ext);

typedef struct {
    const char *name;
    const uint8_t *header;
    size_t header_len;

    SizeParserFunc parse_size; // The parser function. NULL if not used

    const uint8_t *footer;
    size_t footer_len;

    const char *default_ext;       // Default extension
    size_t min_size;
    size_t max_size;
} Filetype;

extern const Filetype REGISTERED_TYPES[];
extern const size_t NUM_REGISTERED_TYPES;

/*static const Filetype JPEG = {{0xFF, 0xD8, 0xFF}, 3, {0xFF, 0xD9}, 2, "jpeg", 30ULL * 1024 * 1204};
static const Filetype GIF = {{0x47, 0x49, 0x46, 0x38, 0x39, 0x61}, 6, {0x00, 0x3B}, 2, "gif", 30ULL * 1024 * 1204};*/

#endif //FILECARVER_FILETYPE_H
