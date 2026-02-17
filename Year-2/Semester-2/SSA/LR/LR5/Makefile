# LR5 Makefile: Memory errors in Linux and tools for their detection
# Run inside Docker or Linux: make all
# Valgrind: valgrind --leak-check=full ./task5.X/binary_name

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean task5.1 task5.2 task5.3 task5.4 task5.5 task5.6 task5.7 task5.8 task5.9 task5.10 variant15

all: task5.1 task5.2 task5.3 task5.4 task5.5 task5.6 task5.7 task5.8 task5.9 task5.10 variant15

task5.1:
	$(CC) $(CFLAGS) task5.1/uninitialized.c -o task5.1/uninitialized

task5.2:
	$(CC) $(CFLAGS) task5.2/out_of_bounds.c -o task5.2/out_of_bounds

task5.3:
	$(CC) $(CFLAGS) task5.3/memory_leak.c -o task5.3/memory_leak

task5.4:
	$(CC) $(CFLAGS) task5.4/undefined_behavior.c -o task5.4/undefined_behavior

task5.5:
	$(CC) $(CFLAGS) task5.5/fragmentation.c -o task5.5/fragmentation

task5.6:
	$(CC) $(CFLAGS) task5.6/dangling_pointer.c -o task5.6/dangling_pointer

task5.7:
	$(CC) $(CFLAGS) task5.7/double_free.c -o task5.7/double_free

task5.8:
	$(CC) $(CFLAGS) -fno-stack-protector task5.8/buffer_overflow.c -o task5.8/buffer_overflow

task5.9:
	$(CC) $(CFLAGS) task5.9/use_after_free.c -o task5.9/use_after_free

task5.10:
	$(CC) $(CFLAGS) task5.10/memory_corruption.c -o task5.10/memory_corruption

variant15:
	$(CC) $(CFLAGS) variant15/cyclic_refs.c -o variant15/cyclic_refs

clean:
	rm -f task5.1/uninitialized task5.2/out_of_bounds task5.3/memory_leak
	rm -f task5.4/undefined_behavior task5.5/fragmentation task5.6/dangling_pointer
	rm -f task5.7/double_free task5.8/buffer_overflow task5.9/use_after_free
	rm -f task5.10/memory_corruption variant15/cyclic_refs
