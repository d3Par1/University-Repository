#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 10.11 (curated): fork + execlp("ls", "-l", NULL). Classic
 * shell-style command execution.
 *
 * Note that execlp's first arg is the program NAME (searched via PATH),
 * while argv[0] is conventionally repeated as the second arg.  Without
 * argv[0], programs like `ls` would see argc=0 and complain.
 */
int main(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        execlp("ls", "ls", "-l", "/", (char *)NULL);
        perror("execlp");          /* only reached if exec FAILS */
        _exit(127);
    }
    int status;
    waitpid(pid, &status, 0);
    printf("\n[parent] ls finished, exit=%d\n", WEXITSTATUS(status));
    return 0;
}
