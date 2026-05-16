#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Task 9.1: Parse `getent passwd` to list accounts on the system, then
 * identify which are "regular" users (UID >= 1000 on modern Linux,
 * vs 500 on RHEL-style — we use 1000) other than the current user.
 *
 * Why popen(getent) instead of fopen("/etc/passwd")?
 *   - getent goes through nsswitch.conf: LDAP, NIS, sssd are visible.
 *   - Reading /etc/passwd directly misses centrally-managed users.
 */

#define MIN_REAL_UID 1000

int main(void)
{
    uid_t self = getuid();
    printf("current uid=%u\n\n", self);

    FILE *p = popen("getent passwd", "r");
    if (!p) { perror("popen"); return 1; }

    char line[1024];
    int regular = 0, total = 0;
    while (fgets(line, sizeof line, p)) {
        char name[256], pw[16], shell[256];
        unsigned uid, gid;
        /* passwd format: name:passwd:uid:gid:gecos:home:shell */
        if (sscanf(line, "%255[^:]:%15[^:]:%u:%u:%*[^:]:%*[^:]:%255s",
                   name, pw, &uid, &gid, shell) >= 4) {
            ++total;
            if (uid >= MIN_REAL_UID && uid != (unsigned)self && uid != 65534 /* nobody */) {
                printf("regular user: %-20s uid=%u  shell=%s\n", name, uid, shell);
                ++regular;
            }
        }
    }
    pclose(p);
    printf("\n%d total accounts, %d regular users besides current\n", total, regular);
    return 0;
}
