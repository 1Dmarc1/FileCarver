#include "parsers.h"
#include <string.h>


ssize_t parse_aiff(const uint8_t *buf, size_t remaining, const char **out_ext) {
    if (remaining < 12 || memcmp(buf, "FORM", 4) != 0) {
        return -1;
    }

    if (memcmp(buf + 8, "AIFF", 4) == 0 || memcmp(buf + 8, "AIFC", 4) == 0) {
        uint32_t payload = ((uint32_t) buf[4] << 24) |
                           ((uint32_t) buf[5] << 16) |
                           ((uint32_t) buf[6] << 8) |
                           (uint32_t) buf[7];

        size_t total_size = (size_t) payload + 8;
        return (ssize_t) total_size;
    }
    return -1;
}


ssize_t parse_riff(const uint8_t *buf, size_t remaining, const char **out_ext) {
    if (!buf || remaining < 12 || memcmp(buf, "RIFF", 4) != 0) {
        return -1;
    }

    uint32_t payload = ((uint32_t) buf[4]) |
                       ((uint32_t) buf[5] << 8) |
                       ((uint32_t) buf[6] << 16) |
                       ((uint32_t) buf[7] << 24);

    size_t total_size = (size_t) payload + 8; // Add the 8 missing bytes from the header

    // Validate size boundaries
    if (total_size < 12 || total_size > remaining) {
        return -1;
    }

    // Check which type of riff file it is
    if (out_ext != NULL) {
        if (memcmp(buf + 8, "WAVE", 4) == 0) {
            *out_ext = "wav";
        } else if (memcmp(buf + 8, "AVI ", 4) == 0) {
            *out_ext = "avi";
        } else if (memcmp(buf + 8, "WEBP", 4) == 0) {
            *out_ext = "webp";
        } else {
            *out_ext = "riff"; // Fallback for unrecognized RIFF types
        }
    }

    return (ssize_t) total_size;
}