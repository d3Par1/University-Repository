#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

/*
 * Task 9.6: Show owner + permissions of files in $HOME, /usr/bin, /etc,
 * then attempt R/W/X access to demonstrate which fail for a regular user.
 */
static void modes(mode_t m, char *out)
{
    out[0] = S_ISDIR(m) ? 'd' : S_ISLNK(m) ? 'l' : '-';
    const char *rwx = "rwxrwxrwx";
    for (int i = 0; i < 9; ++i)
        out[1 + i] = (m & (1 << (8 - i))) ? rwx[i] : '-';
    out[10] = '\0';
}

static void show_dir(const char *dir, int n)
{
    printf("\n== %s (first %d entries) ==\n", dir, n);
    DIR *d = opendir(dir);
    if (!d) { perror(dir); return; }
    struct dirent *e;
    int shown = 0;
    while ((e = readdir(d)) && shown < n) {
        if (e->d_name[0] == '.') continue;
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);
        struct stat st;
        if (lstat(path, &st) < 0) continue;
        char m[11]; modes(st.st_mode, m);
        struct passwd *pw = getpwuid(st.st_uid);
        printf("  %s %-8s %s\n", m, pw ? pw->pw_name : "?", e->d_name);
        ++shown;
    }
    closedir(d);
}

int main(void)
{
    const char *home = getenv("HOME"); if (!home) home = "/tmp";
    show_dir(home, 5);
    show_dir("/usr/bin", 5);
    show_dir("/etc", 5);

    printf("\n== Write probes (should all FAIL for regular user) ==\n");
    int fd;
    fd = open("/etc/passwd", O_WRONLY);
    printf("  open(/etc/passwd, O_WRONLY) → %s\n",
           fd < 0 ? strerror(errno) : "succeeded!?");
    if (fd >= 0) close(fd);
    fd = open("/usr/bin/ls", O_WRONLY);
    printf("  open(/usr/bin/ls, O_WRONLY) → %s\n",
           fd < 0 ? strerror(errno) : "succeeded!?");
    if (fd >= 0) close(fd);

    return 0;
}
