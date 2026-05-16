#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Task 7.6: List only subdirectories of the given path, sorted alphabetically.
 * We use scandir + alphasort which does both jobs in one call. Filter uses
 * stat to keep only directories.
 */
static int dir_only(const struct dirent *e)
{
    if (e->d_name[0] == '.') return 0;
    /* d_type may be DT_UNKNOWN on some filesystems — fall back to lstat */
    if (e->d_type == DT_DIR) return 1;
    if (e->d_type != DT_UNKNOWN) return 0;
    struct stat st;
    return lstat(e->d_name, &st) == 0 && S_ISDIR(st.st_mode);
}

int main(int argc, char **argv)
{
    const char *base = argc > 1 ? argv[1] : ".";
    if (chdir(base) < 0) { perror(base); return 1; }

    struct dirent **names = NULL;
    int n = scandir(".", &names, dir_only, alphasort);
    if (n < 0) { perror("scandir"); return 1; }
    for (int i = 0; i < n; ++i) {
        printf("%s\n", names[i]->d_name);
        free(names[i]);
    }
    free(names);
    return 0;
}
