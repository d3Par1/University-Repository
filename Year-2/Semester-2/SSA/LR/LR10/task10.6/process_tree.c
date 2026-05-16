#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Task 10.8 (curated): build a 3-level process tree where each node spawns
 * 2 children. Total processes:
 *   level 0: 1 (root)
 *   level 1: 2 (children of root)
 *   level 2: 4 (grandchildren) — none of these spawn further
 *   total = 7
 */
static void spawn_subtree(int level, int max)
{
    if (level >= max) return;
    for (int i = 0; i < 2; ++i) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return; }
        if (pid == 0) {
            printf("level=%d  pid=%d  parent=%d\n", level + 1, getpid(), getppid());
            spawn_subtree(level + 1, max);
            /* wait for own descendants before exiting */
            while (wait(NULL) > 0) ;
            _exit(0);
        }
    }
    /* parent: reap both children */
    while (wait(NULL) > 0) ;
}

int main(void)
{
    printf("level=0  pid=%d  (root)\n", getpid());
    spawn_subtree(0, 2);
    return 0;
}
