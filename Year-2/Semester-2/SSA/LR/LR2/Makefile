# Master Makefile for LR2 - All tasks
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

.PHONY: all clean

all: task2.1 task2.2 task2.3 task2.4 task2.5 variant15

task2.1:
	$(CC) $(CFLAGS) -o task2.1/time_end task2.1/time_end.c

task2.2:
	$(CC) $(CFLAGS) -o task2.2/fruts task2.2/fruts.c
	@echo "Also run: bash task2.2/segments.sh"

task2.3:
	$(CC) $(CFLAGS) -o task2.3/stack_location task2.3/stack_location.c

task2.4:
	$(CC) $(CFLAGS) -o task2.4/stacker task2.4/stacker.c

task2.5:
	$(CC) $(CFLAGS) -o task2.5/ip_vs_stack task2.5/ip_vs_stack.c

variant15:
	$(CC) $(CFLAGS) -O0 -o variant15/segment_timing variant15/segment_timing.c -lrt

clean:
	find . -type f -executable -not -name "*.sh" -not -name "*.c" -not -name "*.h" -not -name "Makefile" -delete
	find . -name "*.o" -delete
