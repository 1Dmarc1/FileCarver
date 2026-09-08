#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <getopt.h>

#include "carver.h"


struct option long_options[] = {
        {"input",   required_argument, 0, 'i'},
        {"output",  required_argument, 0, 'o'},
        {"threads", optional_argument, 0, 't'},
        {0, 0, 0, 0}
};

int main(int argc, char *argv[]) {

    const char* input_path;
    const char* output_path;
    int num_threads = 1;

    int opt;
    while ((opt = getopt_long(argc, argv, "i:o:t:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'i':
                input_path = optarg;
                break;
            case 'o':
                output_path = optarg;
                break;
            case 't':
                num_threads = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -i <input_path> -o <output_dir> [-t threads]\n", argv[0]);
                return 1;
        }
    }

    if (input_path == NULL || output_path == NULL) {
        fprintf(stderr, "Error: Missing required arguments.\n");
        fprintf(stderr, "Usage: %s -i <input_path> -o <output_dir> [-t threads]\n", argv[0]);
        return 1;
    }

    int fd = open(input_path, O_RDONLY);
    if (fd < 0){
        perror("Failed to open file %s");
        return 1;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); // Reset to 0

    u_int8_t *map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED){
        perror("Failed to create mapping.");
        close(fd);
        return 1;
    }

    printf("Scanning %s.\n", input_path);
    carve(map, file_size, num_threads, output_path);

    munmap(map, file_size);
    close(fd);
    return 0;
}
