#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

/*
 * Task 7.1: Pipe `who` (we use `who` instead of `rwho` because rwhod is
 * obsolete and rarely installed) into `more` using popen + pipe.
 *
 * popen("who") gives us a FILE* connected to `who`'s stdout. We then push
 * those bytes into another popen("more") whose stdin we write to. This
 * mirrors a shell pipeline `who | more` without exec'ing /bin/sh.
 */
int main(void)
{
    FILE *who = popen("who", "r");
    if (!who) { perror("popen who"); return 1; }

    FILE *more = popen("more -20", "w");
    if (!more) { perror("popen more"); pclose(who); return 1; }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof buf, who)) > 0) {
        fwrite(buf, 1, n, more);
    }

    pclose(who);
    pclose(more);
    return 0;
}
