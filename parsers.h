#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef FILECARVER_PARSERS_H
#define FILECARVER_PARSERS_H


ssize_t parse_aiff(const uint8_t *buf, size_t remaining, const char **out_ext);


ssize_t parse_riff(const uint8_t *buf, size_t remaining, const char **out_ext);


#endif //FILECARVER_PARSERS_H
