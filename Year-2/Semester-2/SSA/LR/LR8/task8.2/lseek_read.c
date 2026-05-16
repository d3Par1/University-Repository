#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * Task 8.2: File contains bytes 4,5,2,2,3,3,7,9,1,5
 *           lseek(fd, 3, SEEK_SET);
 *           read(fd, buf, 4);
 * What's in buf?
 *
 * lseek moves the position to byte index 3 (zero-based). Then read reads
 * the next 4 bytes starting from index 3: bytes at positions 3,4,5,6 →
 * 2, 3, 3, 7.
 */
int main(void)
{
    const unsigned char data[] = {4, 5, 2, 2, 3, 3, 7, 9, 1, 5};

    /* create scratch file in /tmp */
    char path[] = "/tmp/lr8_t82_XXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) { perror("mkstemp"); return 1; }
    if (write(fd, data, sizeof data) != (ssize_t)sizeof data) {
        perror("write"); close(fd); unlink(path); return 1;
    }

    /* the experiment */
    if (lseek(fd, 3, SEEK_SET) == (off_t)-1) { perror("lseek"); return 1; }
    unsigned char buf[4] = {0};
    ssize_t n = read(fd, buf, sizeof buf);
    printf("file contents: ");
    for (size_t i = 0; i < sizeof data; ++i) printf("%d ", data[i]);
    putchar('\n');
    printf("lseek(fd, 3, SEEK_SET) — position now at index 3\n");
    printf("read(fd, buf, 4)       — read 4 bytes from index 3\n");
    printf("buffer (n=%zd):  ", n);
    for (int i = 0; i < n; ++i) printf("%d ", buf[i]);
    putchar('\n');
    printf("→ expected: 2 3 3 7  (bytes at index 3,4,5,6)\n");

    close(fd);
    unlink(path);
    return 0;
}
