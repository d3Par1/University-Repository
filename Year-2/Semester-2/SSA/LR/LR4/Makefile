# LR4 Makefile: Memory allocation (malloc)
# Run inside Docker or Linux: make all

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

.PHONY: all clean task4.1 task4.2 task4.3 task4.4 task4.5 task4.6 task4.7 variant15

all: task4.1 task4.2 task4.3 task4.4 task4.5 task4.6 task4.7 variant15

task4.1:
	$(CC) $(CFLAGS) task4.1/max_malloc.c -o task4.1/max_malloc

task4.2:
	$(CC) $(CFLAGS) task4.2/negative_malloc.c -o task4.2/negative_malloc

task4.3:
	$(CC) $(CFLAGS) task4.3/malloc_zero.c -o task4.3/malloc_zero

task4.4:
	$(CC) $(CFLAGS) -g task4.4/malloc_bug.c -o task4.4/malloc_bug

task4.5:
	$(CC) $(CFLAGS) -D_GNU_SOURCE task4.5/realloc_fail.c -o task4.5/realloc_fail

task4.6:
	$(CC) $(CFLAGS) task4.6/realloc_null_zero.c -o task4.6/realloc_null_zero

task4.7:
	$(CC) $(CFLAGS) -D_GNU_SOURCE task4.7/reallocarray_test.c -o task4.7/reallocarray_test

variant15:
	$(CC) $(CFLAGS) variant15/malloc_wrapper.c -o variant15/malloc_wrapper -Wl,--wrap=malloc,--wrap=free

clean:
	rm -f task4.1/max_malloc task4.2/negative_malloc task4.3/malloc_zero
	rm -f task4.4/malloc_bug task4.5/realloc_fail task4.6/realloc_null_zero
	rm -f task4.7/reallocarray_test variant15/malloc_wrapper
