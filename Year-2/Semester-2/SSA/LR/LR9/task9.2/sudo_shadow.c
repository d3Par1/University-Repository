#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * Task 9.2: Run `cat /etc/shadow` as root from a regular user account,
 * assuming sudo is configured.
 *
 * /etc/shadow is mode 0640 owned by root:shadow, so a regular user
 * cannot read it. The legitimate paths are:
 *   1) sudo  — temporary privilege escalation
 *   2) setuid root binary — permanent escalation
 *   3) capabilities (CAP_DAC_READ_SEARCH)
 *
 * We use approach (1) via execvp("sudo", ...). On a properly configured
 * system this prompts for a password.
 */
int main(void)
{
    /* First: attempt direct read — should fail with EACCES */
    int fd = open("/etc/shadow", O_RDONLY);
    if (fd >= 0) {
        printf("WARNING: /etc/shadow is readable by uid=%u — misconfig!\n", getuid());
        close(fd);
    } else {
        printf("direct open /etc/shadow as uid=%u → %m  (expected)\n", getuid());
    }

    printf("\nNow trying via sudo (will prompt or fail if no sudo rights):\n");
    fflush(stdout);

    /* run: sudo head -3 /etc/shadow */
    char *const argv[] = { "sudo", "-n", "head", "-3", "/etc/shadow", NULL };
    execvp("sudo", argv);
    perror("execvp sudo");
    return 1;
}
