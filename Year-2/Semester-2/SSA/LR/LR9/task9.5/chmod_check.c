#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * Task 9.5: Create a temp file as regular user, then chmod through several
 * modes and check which give read/write access from the regular account.
 *
 * The check is access(R_OK|W_OK) which uses the EFFECTIVE UID, but is
 * meant for setuid binaries to know what the REAL user can do — see
 * faccessat(AT_EACCESS) for true effective-uid check. Here both are
 * the same because we don't have setuid.
 */
static void probe(const char *path)
{
    struct stat st;
    if (stat(path, &st) < 0) { perror("stat"); return; }
    int r = access(path, R_OK);
    int w = access(path, W_OK);
    printf("  mode=%04o  R_OK=%s  W_OK=%s\n",
           st.st_mode & 07777,
           r == 0 ? "yes" : "no",
           w == 0 ? "yes" : "no");
}

int main(void)
{
    char path[] = "/tmp/lr9_t95_XXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) { perror("mkstemp"); return 1; }
    (void)!write(fd, "data\n", 5);
    close(fd);

    printf("Created %s\n\n", path);

    mode_t modes[] = { 0644, 0600, 0400, 0044, 0000 };
    const char *desc[] = {
        "rw-r--r--  (default)",
        "rw-------  (owner only)",
        "r--------  (read-only)",
        "----r--r--  (no owner access!)",
        "----------  (no access)"
    };
    for (size_t i = 0; i < sizeof modes / sizeof modes[0]; ++i) {
        chmod(path, modes[i]);
        printf("chmod %04o (%s):\n", modes[i], desc[i]);
        probe(path);
    }

    /* Restore permissions before unlink */
    chmod(path, 0644);
    unlink(path);

    printf("\nKey insight: mode 0044 has NO bits for the owner — but the owner\n"
           "can still CHMOD it back (chmod is allowed for the owner regardless\n"
           "of permission bits — that's why the lock-out is not permanent).\n");
    return 0;
}
