#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>

/*
 * Task 9.4: Replicate `whoami` and `id` to show the user identity and group
 * memberships of the current process.
 *
 * The kernel maintains: real UID, effective UID, saved UID (similarly for
 * GID), plus supplementary groups. `whoami` shows effective UID.
 * Supplementary groups come from initgroups()/setgroups() at login, and
 * are inherited by every child of that login shell.
 */
int main(void)
{
    uid_t r, e, s;
    gid_t rg, eg, sg;
    getresuid(&r, &e, &s);
    getresgid(&rg, &eg, &sg);

    struct passwd *pw = getpwuid(e);
    struct group  *gr = getgrgid(eg);

    printf("== whoami ==\n");
    printf("  %s\n", pw ? pw->pw_name : "<unknown>");

    printf("\n== id ==\n");
    printf("  uid=%u(%s) gid=%u(%s)\n",
           e, pw ? pw->pw_name : "?",
           eg, gr ? gr->gr_name : "?");
    if (r != e) printf("  real_uid=%u  (differs from effective)\n", r);
    if (s != e) printf("  saved_uid=%u (differs)\n", s);

    gid_t groups[64];
    int n = getgroups(64, groups);
    printf("  supplementary groups (%d): ", n);
    for (int i = 0; i < n; ++i) {
        struct group *g = getgrgid(groups[i]);
        printf("%u(%s) ", groups[i], g ? g->gr_name : "?");
    }
    putchar('\n');
    return 0;
}
