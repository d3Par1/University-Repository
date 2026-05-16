#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Variant 15: print directory hierarchy starting from a root.
 *
 *   - We KEEP symbolic links (printed with -> target).
 *   - We EXCLUDE cycles. A cycle on UNIX is detected by tracking the set
 *     of (st_dev, st_ino) pairs of directories on the current path from
 *     the root. If we hit a directory whose (dev, ino) is already on the
 *     stack — we are looping (typical case: a symlink that points back to
 *     an ancestor, or a bind mount).
 *
 * We start from the user's $HOME by default — running this on '/' produces
 * tens of thousands of lines and noisy permission errors. Pass the path
 * as argv[1] to override.
 */

typedef struct {
    dev_t dev;
    ino_t ino;
} DirKey;

#define MAX_DEPTH 1024
static DirKey stack[MAX_DEPTH];
static int stack_top = 0;

static int on_stack(dev_t dev, ino_t ino)
{
    for (int i = 0; i < stack_top; ++i)
        if (stack[i].dev == dev && stack[i].ino == ino) return 1;
    return 0;
}

static void indent(int depth)
{
    for (int i = 0; i < depth; ++i) fputs("  ", stdout);
}

static void walk(const char *path, int depth)
{
    struct stat st;
    if (lstat(path, &st) < 0) return;

    indent(depth);
    if (S_ISLNK(st.st_mode)) {
        /* print symlink with its target */
        char target[1024];
        ssize_t n = readlink(path, target, sizeof target - 1);
        if (n < 0) n = 0;
        target[n] = '\0';
        printf("%s -> %s\n", path, target);
        return;
    }
    printf("%s\n", path);
    if (!S_ISDIR(st.st_mode)) return;

    /* Cycle check: did we already enter this directory on the current path? */
    if (on_stack(st.st_dev, st.st_ino)) {
        indent(depth + 1);
        printf("[cycle ignored: dev=%lu ino=%lu]\n",
               (unsigned long)st.st_dev, (unsigned long)st.st_ino);
        return;
    }
    if (stack_top >= MAX_DEPTH) {
        indent(depth + 1);
        printf("[depth limit]\n");
        return;
    }
    stack[stack_top++] = (DirKey){ st.st_dev, st.st_ino };

    DIR *d = opendir(path);
    if (!d) {
        indent(depth + 1);
        printf("[opendir: %s]\n", strerror(errno));
        --stack_top;
        return;
    }
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
        char child[2048];
        snprintf(child, sizeof child, "%s/%s", path, e->d_name);
        walk(child, depth + 1);
    }
    closedir(d);
    --stack_top;
}

int main(int argc, char **argv)
{
    (void)argc;
    const char *root = argv[1] ? argv[1] : getenv("HOME");
    if (!root) root = "/";
    walk(root, 0);
    return 0;
}
