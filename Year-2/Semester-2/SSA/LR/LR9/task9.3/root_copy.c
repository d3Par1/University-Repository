#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

/*
 * Task 9.3: Regular user creates a file. Then "root" copies it into the
 * user's home (we simulate root by using sudo). Then the regular user
 * tries to modify and delete the copy.
 *
 * Key facts the demo establishes:
 *   - sudo cp preserves NOT the owner (sudo runs as root → new file owned
 *     by root). Need `sudo cp --preserve=ownership` to keep regular owner.
 *   - User cannot WRITE a file owned by root if it's mode 0644.
 *   - But user CAN UNLINK a root-owned file if the user OWNS the directory!
 *     This is a classic Unix surprise: delete permission is on the parent
 *     directory, not on the file.
 */
static void show(const char *path)
{
    struct stat st;
    if (stat(path, &st) < 0) { perror(path); return; }
    printf("  %s: uid=%u mode=%o size=%lld\n",
           path, st.st_uid, st.st_mode & 07777, (long long)st.st_size);
}

int main(void)
{
    const char *src  = "/tmp/lr9_t93_src.txt";
    const char *dst  = "/tmp/lr9_t93_root_copy.txt";

    /* Step 1: regular user creates source */
    FILE *f = fopen(src, "w");
    if (!f) { perror("fopen"); return 1; }
    fputs("Created by regular user\n", f);
    fclose(f);
    printf("Step 1: regular user created %s\n", src);
    show(src);

    /* Step 2: sudo cp (must succeed without password if sudo NOPASSWD,
       else prompts).  We use -n (non-interactive) so the demo doesn't hang. */
    printf("\nStep 2: sudo -n cp %s %s\n", src, dst);
    pid_t pid = fork();
    if (pid == 0) {
        execlp("sudo", "sudo", "-n", "cp", src, dst, (char *)NULL);
        perror("execlp"); _exit(1);
    }
    int status; waitpid(pid, &status, 0);
    if (WEXITSTATUS(status) != 0) {
        printf("  (sudo not available non-interactively; simulating as user-owned copy)\n");
        /* Fallback: just copy ourselves so the rest of the demo runs */
        FILE *r = fopen(src, "r"), *w = fopen(dst, "w");
        if (r && w) { int c; while ((c = fgetc(r)) != EOF) fputc(c, w); }
        if (r) fclose(r);
        if (w) fclose(w);
    }
    show(dst);

    /* Step 3: regular user tries to write to dst */
    printf("\nStep 3: regular user opens %s O_WRONLY\n", dst);
    int fd = open(dst, O_WRONLY | O_APPEND);
    if (fd < 0) {
        printf("  open() failed: %m  (expected if file owned by root, mode 0644)\n");
    } else {
        printf("  open() succeeded — file is writable by current user\n");
        (void)!write(fd, "appended\n", 9);
        close(fd);
    }

    /* Step 4: regular user tries to delete via rm */
    printf("\nStep 4: regular user runs unlink(%s)\n", dst);
    if (unlink(dst) == 0) {
        printf("  unlink() succeeded — directory /tmp grants write perm to user.\n");
        printf("  (Delete permission is on the PARENT DIR, not on the file itself.)\n");
    } else {
        printf("  unlink() failed: %m\n");
    }

    unlink(src);
    return 0;
}
