#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void target_function(int iter)
{
    printf("[target] inside target_function, iter=%d\n", iter);
}

int main(void)
{
    printf("[target] started, pid=%d\n", getpid());
    for (int i = 0; i < 3; ++i) {
        target_function(i);
        sleep(1);
    }
    printf("[target] exiting normally\n");
    return 0;
}
