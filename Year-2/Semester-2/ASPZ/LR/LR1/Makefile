# Master Makefile for LR1 - All tasks
# Run from WSL: cd /mnt/d/... && make all
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

.PHONY: all clean

all: task1.1 task1.6 task1.7 task1.8 task1.9 task1.10 task1.11 \
     task1.12 task1.13 task1.14 task1.15 task1.16 task1.17 task1.18 \
     task1.19 task1.20 task1.21 task1.22 task1.23 task1.24 task1.25

task1.1:
	$(MAKE) -C task1.1

task1.6:
	$(CC) $(CFLAGS) -o task1.6/task1_6 task1.6/seconds_converter.c

task1.7:
	$(CC) $(CFLAGS) -o task1.7/task1_7 task1.7/base_converter.c -lm

task1.8:
	$(CC) $(CFLAGS) -o task1.8/task1_8 task1.8/array_search.c -lm

task1.9:
	$(CC) $(CFLAGS) -o task1.9/task1_9 task1.9/string_replace.c

task1.10:
	$(CC) $(CFLAGS) -o task1.10/task1_10 task1.10/tomorrow_date.c

task1.11:
	$(CC) $(CFLAGS) -o task1.11/task1_11 task1.11/student_db.c

task1.12:
	$(CC) $(CFLAGS) -o task1.12/task1_12 task1.12/generic_array.c

task1.13:
	$(CC) $(CFLAGS) -o task1.13/task1_13 task1.13/dynamic_array.c

task1.14:
	$(CC) $(CFLAGS) -o task1.14/last task1.14/last.c

task1.15:
	$(CC) $(CFLAGS) -fopenmp -o task1.15/task1_15 task1.15/sorting.c

task1.16:
	$(CC) $(CFLAGS) -o task1.16/task1_16 task1.16/struct_sort.c

task1.17:
	$(CC) $(CFLAGS) -o task1.17/task1_17 task1.17/insort.c

task1.18:
	$(CC) $(CFLAGS) -o task1.18/task1_18 task1.18/binary_print.c

task1.19:
	$(CC) $(CFLAGS) -o task1.19/task1_19 task1.19/char_freq.c

task1.20:
	$(CC) $(CFLAGS) -o task1.20/task1_20 task1.20/csv_table.c

task1.21:
	$(CC) $(CFLAGS) -o task1.21/task1_21 task1.21/equation_roots.c -lm

task1.22:
	$(CC) $(CFLAGS) -pthread -o task1.22/task1_22 task1.22/circular_buffer.c

task1.23:
	$(CC) $(CFLAGS) -o task1.23/task1_23 task1.23/bit_invert.c

task1.24:
	$(CC) $(CFLAGS) -o task1.24/task1_24 task1.24/swap_macro.c

task1.25:
	$(CC) $(CFLAGS) -o task1.25/task1_25 task1.25/random_float.c

task1.5:
	$(CC) $(CFLAGS) -o task1.5/task1_5 task1.5/dependencies.c

clean:
	find . -type f -executable -not -name "*.sh" -not -name "*.c" -not -name "*.h" -not -name "Makefile" -not -name "CMakeLists.txt" -not -name "*.csv" -delete
	find . -name "*.o" -delete
