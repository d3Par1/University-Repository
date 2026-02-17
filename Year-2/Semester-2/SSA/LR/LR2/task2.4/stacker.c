/*
 * Task 2.4 - Stack analysis with GDB (stacker program from lecture)
 *
 * Compile: gcc -g -Wall -o stacker stacker.c
 *
 * Usage:
 *   1) Run in background: ./stacker &
 *   2) Get PID:           echo $!
 *   3) Use gstack:        gstack <PID>  (or pstack <PID>)
 *   4) Use GDB:           gdb -q -p <PID>
 *      (gdb) bt
 *      (gdb) info frame
 *      (gdb) info locals
 *      (gdb) detach
 *      (gdb) quit
 *   5) Kill:              kill <PID>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define MSG "In function %20s; &localvar = %p\n"

static void bar_is_now_closed(void) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, (void *)&localvar);
    printf("\n Now blocking on pause()...\n");
    printf(" PID: %d\n", getpid());
    printf(" Attach with: gdb -q -p %d\n", getpid());
    printf(" Or use:      pstack %d\n\n", getpid());
    fflush(stdout);
    pause();  /* Block until signal received */
}

static void bar(void) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, (void *)&localvar);
    bar_is_now_closed();
}

static void foo(void) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, (void *)&localvar);
    bar();
}

int main(int argc, char **argv) {
    int localvar = 5;
    printf(MSG, __FUNCTION__, (void *)&localvar);
    foo();
    exit(EXIT_SUCCESS);
}
