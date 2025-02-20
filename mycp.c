#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

void print_usage() {
    fprintf(stderr, "USAGE: ./mycp source_file target_file\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
        exit(3);
    }

    const char *source_file = argv[1];
    const char *target_file = argv[2];

    struct stat statbuf;
    if (stat(source_file, &statbuf) != 0) {
        perror("Error accessing source file");
        exit(4);
    }

    if (!S_ISREG(statbuf.st_mode)) {
        fprintf(stderr, "Error: Source file is not a regular file\n");
        exit(5);
    }

    int src_fd = open(source_file, O_RDONLY);
    if (src_fd < 0) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    int dest_fd = open(target_file, O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode);
    if (dest_fd < 0) {
        perror("Error opening target file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to target file");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read < 0) {
        perror("Error reading source file");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }

    close(src_fd);
    close(dest_fd);
    return EXIT_SUCCESS;
}
