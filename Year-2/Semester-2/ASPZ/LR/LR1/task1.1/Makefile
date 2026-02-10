CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lm

# Default target
all: task1_1

# Build with O2 optimization
task1_1: main.c erf_module.c erf_module.h
	$(CC) $(CFLAGS) -O2 -o $@ main.c erf_module.c $(LDFLAGS)

# Build with O3 optimization (for performance comparison)
task1_1_o3: main.c erf_module.c erf_module.h
	$(CC) $(CFLAGS) -O3 -o $@ main.c erf_module.c $(LDFLAGS)

# Build with debug info
task1_1_dbg: main.c erf_module.c erf_module.h
	$(CC) $(CFLAGS) -g -O0 -o $@ main.c erf_module.c $(LDFLAGS)

# Performance comparison
perf: task1_1 task1_1_o3
	@echo "=== O2 optimization ==="
	time echo "1.96" | ./task1_1
	@echo "\n=== O3 optimization ==="
	time echo "1.96" | ./task1_1_o3

clean:
	rm -f task1_1 task1_1_o3 task1_1_dbg *.o

.PHONY: all perf clean
