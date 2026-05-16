#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

/*
 * Task 7.2: ls -l clone built from scratch via opendir/readdir/lstat.
 * We are NOT allowed to exec /bin/ls. We must format mode bits, resolve
 * UID -> username via getpwuid, GID -> groupname via getgrgid, and format
 * mtime ourselves.
 */
static void mode_str(mode_t m, char *out)
{
    out[0] = S_ISDIR(m) ? 'd' : S_ISLNK(m) ? 'l' : '-';
    out[1] = (m & S_IRUSR) ? 'r' : '-';
    out[2] = (m & S_IWUSR) ? 'w' : '-';
    out[3] = (m & S_IXUSR) ? 'x' : '-';
    out[4] = (m & S_IRGRP) ? 'r' : '-';
    out[5] = (m & S_IWGRP) ? 'w' : '-';
    out[6] = (m & S_IXGRP) ? 'x' : '-';
    out[7] = (m & S_IROTH) ? 'r' : '-';
    out[8] = (m & S_IWOTH) ? 'w' : '-';
    out[9] = (m & S_IXOTH) ? 'x' : '-';
    out[10] = '\0';
}

int main(int argc, char **argv)
{
    const char *dir = (argc > 1) ? argv[1] : ".";
    DIR *d = opendir(dir);
    if (!d) { perror(dir); return 1; }

    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.') continue;   /* skip dotfiles */

        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);

        struct stat st;
        if (lstat(path, &st) < 0) { perror(path); continue; }

        char modes[11];
        mode_str(st.st_mode, modes);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);

        char timebuf[64];
        struct tm *tm = localtime(&st.st_mtime);
        strftime(timebuf, sizeof timebuf, "%b %d %H:%M", tm);

        printf("%s %2lu %-8s %-8s %8lld %s %s\n",
               modes,
               (unsigned long)st.st_nlink,
               pw ? pw->pw_name : "?",
               gr ? gr->gr_name : "?",
               (long long)st.st_size,
               timebuf,
               e->d_name);
    }
    closedir(d);
    return 0;
}
