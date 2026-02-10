// Завдання 1.14: Реалізація команди last
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEFAULT_LINES 10
#define MAX_LINE_LEN 4096

void last_lines_ring(FILE *fp, int n, int reverse) {
    char **ring = calloc(n, sizeof(char *));
    if (!ring) { perror("calloc"); return; }

    for (int i = 0; i < n; i++) {
        ring[i] = malloc(MAX_LINE_LEN);
        if (!ring[i]) { perror("malloc"); return; }
        ring[i][0] = '\0';
    }

    int idx = 0, total = 0;
    char buf[MAX_LINE_LEN];

    while (fgets(buf, MAX_LINE_LEN, fp)) {
        strncpy(ring[idx % n], buf, MAX_LINE_LEN - 1);
        ring[idx % n][MAX_LINE_LEN - 1] = '\0';
        idx++;
        total++;
    }

    int count = (total < n) ? total : n;
    int start = (total < n) ? 0 : idx % n;

    if (reverse) {
        for (int i = count - 1; i >= 0; i--) {
            printf("%s", ring[(start + i) % n]);
        }
    } else {
        for (int i = 0; i < count; i++) {
            printf("%s", ring[(start + i) % n]);
        }
    }

    for (int i = 0; i < n; i++) free(ring[i]);
    free(ring);
}

void last_lines_mmap(const char *filename, int n, int reverse) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    struct stat st;
    if (fstat(fd, &st) < 0) { perror("fstat"); close(fd); return; }

    if (st.st_size == 0) { close(fd); return; }

    char *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); close(fd); return; }

    long *line_starts = malloc((n + 1) * sizeof(long));
    int line_count = 0;
    long pos = st.st_size - 1;

    if (pos >= 0 && data[pos] == '\n') pos--;

    while (pos >= 0 && line_count < n) {
        if (data[pos] == '\n') {
            line_starts[line_count++] = pos + 1;
        }
        pos--;
    }
    if (pos < 0 && line_count < n) {
        line_starts[line_count++] = 0;
    }

    if (reverse) {
        for (int i = 0; i < line_count; i++) {
            long start = line_starts[i];
            long end = (i == 0) ? st.st_size : line_starts[i - 1];
            fwrite(data + start, 1, end - start, stdout);
        }
    } else {
        for (int i = line_count - 1; i >= 0; i--) {
            long start = line_starts[i];
            long end = (i == 0) ? st.st_size : line_starts[i - 1];
            fwrite(data + start, 1, end - start, stdout);
        }
    }

    free(line_starts);
    munmap(data, st.st_size);
    close(fd);
}

int main(int argc, char *argv[]) {
    int n = DEFAULT_LINES;
    int reverse = 0;
    const char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            n = atoi(argv[++i]);
            if (n <= 0) n = DEFAULT_LINES;
        } else if (strcmp(argv[i], "-r") == 0) {
            reverse = 1;
        } else {
            filename = argv[i];
        }
    }

    if (filename) {
        last_lines_mmap(filename, n, reverse);
    } else {
        last_lines_ring(stdin, n, reverse);
    }

    return 0;
}
