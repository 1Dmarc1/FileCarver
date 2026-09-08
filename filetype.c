#include "filetype.h"
#include "parsers.h"


const Filetype REGISTERED_TYPES[] = {
        // JPEG
        {
                .name         = "JPEG",
                .header       = (const uint8_t[]){0xFF, 0xD8, 0xFF},
                .header_len   = 3,
                .footer       = (const uint8_t[]){0xFF, 0xD9},
                .footer_len   = 2,
                .default_ext  = "jpg",
                .min_size     = 512,
                .max_size     = 25ULL * 1024 * 1024
        },
        // PNG
        {
                .name         = "PNG",
                .header       = (const uint8_t[]){0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A},
                .header_len   = 8,
                .footer       = (const uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82},
                .footer_len   = 12,
                .default_ext  = "png",
                .min_size     = 512,
                .max_size     = 25ULL * 1024 * 1024
        },
        // PDF
        {
                .name         = "PDF",
                .header       = (const uint8_t[]){0x25, 0x50, 0x44, 0x46},
                .header_len   = 4,
                .footer       = (const uint8_t[]){0x25, 0x25, 0x45, 0x4F, 0x46},
                .footer_len   = 5,
                .default_ext  = "pdf",
                .min_size     = 512,
                .max_size     = 25ULL * 1024 * 1024
        },
        // AIFF
        {
                .name         = "AIFF",
                .header       = (const uint8_t[]){"FORM"},
                .header_len   = 4,
                .parse_size   = parse_aiff,
                .footer       = (const uint8_t[]){},
                .footer_len   = 0,
                .default_ext  = "aiff",
                .min_size     = 512,
                .max_size     = 25ULL * 1024 * 1024
        },
        // RIFF
        {
                .name         = "RIFF",
                .header       = (const uint8_t[]){"RIFF"},
                .header_len   = 4,
                .parse_size   = parse_riff,
                .footer       = (const uint8_t[]){},
                .footer_len   = 0,
                .default_ext  = "riff",
                .min_size     = 512,
                .max_size     = 25ULL * 1024 * 1024
        },
};

const size_t NUM_REGISTERED_TYPES = sizeof(REGISTERED_TYPES) / sizeof(REGISTERED_TYPES[0]);