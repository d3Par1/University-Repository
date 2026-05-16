#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 * Task 7.8: For each regular file in the directory ask "delete?".
 * unlink() removes the directory entry — if no other process holds the
 * inode open, the file is freed. We skip directories.
 */
static int ask(const char *name)
{
    FILE *tty = fopen("/dev/tty", "r");
    if (!tty) tty = stdin;
    fprintf(stderr, "Delete '%s'? [y/N] ", name);
    char buf[16] = {0};
    if (!fgets(buf, sizeof buf, tty)) { if (tty != stdin) fclose(tty); return 0; }
    if (tty != stdin) fclose(tty);
    return buf[0] == 'y' || buf[0] == 'Y';
}

int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : ".";
    DIR *d = opendir(dir);
    if (!d) { perror(dir); return 1; }

    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.') continue;
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);
        struct stat st;
        if (lstat(path, &st) < 0) continue;
        if (!S_ISREG(st.st_mode)) continue;
        if (ask(path)) {
            if (unlink(path) == 0) printf("removed %s\n", path);
            else perror("unlink");
        }
    }
    closedir(d);
    return 0;
}
