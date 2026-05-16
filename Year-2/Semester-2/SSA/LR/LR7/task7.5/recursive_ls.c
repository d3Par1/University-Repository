#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

/*
 * Task 7.5: Recursively list every file under the given directory.
 * We use lstat so symlinks themselves are reported (we don't follow them
 * — otherwise a self-referential symlink would loop forever).
 */
static void walk(const char *dir, int depth)
{
    DIR *d = opendir(dir);
    if (!d) { perror(dir); return; }

    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;

        char path[2048];
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);

        for (int i = 0; i < depth; ++i) fputs("  ", stdout);
        puts(path);

        struct stat st;
        if (lstat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            walk(path, depth + 1);
        }
    }
    closedir(d);
}

int main(int argc, char **argv)
{
    walk(argc > 1 ? argv[1] : ".", 0);
    return 0;
}
