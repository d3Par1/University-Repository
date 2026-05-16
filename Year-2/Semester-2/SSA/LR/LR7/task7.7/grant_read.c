#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Task 7.7: Iterate the user's *.c files in the current directory, show
 * each one, ask "grant other-read?" and chmod o+r if yes.
 *
 * Reading from /dev/tty so stdin can be piped (for non-interactive demo).
 */
static int ask(const char *name)
{
    FILE *tty = fopen("/dev/tty", "r");
    if (!tty) tty = stdin;
    fprintf(stderr, "Grant other-read on %s? [y/N] ", name);
    char buf[16] = {0};
    if (!fgets(buf, sizeof buf, tty)) {
        if (tty != stdin) fclose(tty);
        return 0;
    }
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
        size_t L = strlen(e->d_name);
        if (L < 3 || strcmp(e->d_name + L - 2, ".c") != 0) continue;

        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);
        struct stat st;
        if (stat(path, &st) < 0) continue;
        if (st.st_mode & S_IROTH) {
            printf("%s already world-readable\n", path);
            continue;
        }
        if (ask(path)) {
            if (chmod(path, st.st_mode | S_IROTH) == 0)
                printf("granted o+r on %s\n", path);
            else
                perror("chmod");
        }
    }
    closedir(d);
    return 0;
}
