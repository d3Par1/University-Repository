/*
 * Task 3.6: Stack size limit demonstration (RLIMIT_STACK)
 *
 * A recursive program that actively uses the stack.
 * Demonstrates hitting the stack size limit.
 *
 * Compile: gcc -Wall -Wextra -std=c11 stack_limit.c -o stack_limit
 * Run:     ./stack_limit [stack_size_kb]
 * Example: ./stack_limit 256
 *
 * Docker: docker run --rm -it ubuntu bash
 *         ulimit -s 256 && ./stack_limit
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <setjmp.h>

static sigjmp_buf jump_buf;
static volatile sig_atomic_t segfault_caught = 0;
static int max_depth = 0;

static void sigsegv_handler(int sig) {
    (void)sig;
    segfault_caught = 1;
    siglongjmp(jump_buf, 1);
}

/*
 * Recursive function that allocates a large local buffer each call.
 * Each frame uses ~1KB of stack space.
 */
static void recurse(int depth) {
    char buffer[1024]; /* 1 KB per stack frame */
    memset(buffer, (char)depth, sizeof(buffer));

    if (depth > max_depth)
        max_depth = depth;

    if (depth % 100 == 0) {
        printf("  Depth: %d, stack frame at: %p\n", depth, (void *)buffer);
    }

    /* Prevent compiler from optimizing out buffer */
    if (buffer[0] == 127 && buffer[1023] == 127)
        printf("  (unlikely)\n");

    recurse(depth + 1);
}

int main(int argc, char *argv[]) {
    printf("=== Task 3.6: Stack Size Limit Demonstration ===\n\n");

    /* Show current stack limit */
    struct rlimit rl;
    getrlimit(RLIMIT_STACK, &rl);
    printf("Current stack limits:\n");
    printf("  Soft: %lu KB (%lu bytes)\n",
           (unsigned long)(rl.rlim_cur / 1024),
           (unsigned long)rl.rlim_cur);
    if (rl.rlim_max == RLIM_INFINITY)
        printf("  Hard: unlimited\n");
    else
        printf("  Hard: %lu KB\n", (unsigned long)(rl.rlim_max / 1024));

    /* Set custom stack limit if argument provided */
    if (argc >= 2) {
        long stack_kb = atol(argv[1]);
        if (stack_kb > 0) {
            rl.rlim_cur = (rlim_t)(stack_kb * 1024);
            if (setrlimit(RLIMIT_STACK, &rl) != 0) {
                perror("setrlimit STACK");
                return 1;
            }
            printf("\nStack limit set to %ld KB\n", stack_kb);
        }
    }

    /* Get updated limit */
    getrlimit(RLIMIT_STACK, &rl);
    printf("\nEffective stack limit: %lu KB\n", (unsigned long)(rl.rlim_cur / 1024));
    printf("Each recursion frame: ~1 KB\n");
    printf("Expected max depth: ~%lu\n\n",
           (unsigned long)(rl.rlim_cur / 1024));

    /* Install SIGSEGV handler to catch stack overflow */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /* Use alternate signal stack so handler works during stack overflow */
    stack_t ss;
    ss.ss_sp = malloc(SIGSTKSZ);
    if (!ss.ss_sp) {
        perror("malloc sigstack");
        return 1;
    }
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, NULL) != 0) {
        perror("sigaltstack");
        free(ss.ss_sp);
        return 1;
    }
    sa.sa_flags = SA_ONSTACK;

    if (sigaction(SIGSEGV, &sa, NULL) != 0) {
        perror("sigaction");
        free(ss.ss_sp);
        return 1;
    }

    printf("Starting recursive descent...\n\n");

    if (sigsetjmp(jump_buf, 1) == 0) {
        recurse(1);
    } else {
        /* Returned from signal handler */
        printf("\n*** Stack overflow caught (SIGSEGV)! ***\n");
    }

    printf("\nMax recursion depth reached: %d\n", max_depth);
    printf("Approximate stack used: %d KB\n", max_depth);
    printf("Stack limit was: %lu KB\n", (unsigned long)(rl.rlim_cur / 1024));

    free(ss.ss_sp);
    return 0;
}
