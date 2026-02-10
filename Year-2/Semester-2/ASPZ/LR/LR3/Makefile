# LR3 Makefile: Docker resource limits
# Run inside Docker: docker run -it --rm -v $(pwd):/src -w /src ubuntu bash
# Then: apt update && apt install -y build-essential && make all

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS_RT = -lrt

.PHONY: all clean task3.2 task3.3 task3.4 task3.5 task3.6 variant15

all: task3.2 task3.3 task3.4 task3.5 task3.6 variant15

task3.2:
	$(CC) $(CFLAGS) task3.2/perf_limit.c -o task3.2/perf_limit

task3.3:
	$(CC) $(CFLAGS) task3.3/dice_sim.c -o task3.3/dice_sim

task3.4:
	$(CC) $(CFLAGS) task3.4/lottery.c -o task3.4/lottery

task3.5:
	$(CC) $(CFLAGS) task3.5/filecopy.c -o task3.5/filecopy

task3.6:
	$(CC) $(CFLAGS) task3.6/stack_limit.c -o task3.6/stack_limit

variant15:
	$(CC) $(CFLAGS) variant15/ulimit_fsize.c -o variant15/ulimit_fsize $(LDFLAGS_RT)

clean:
	rm -f task3.2/perf_limit task3.3/dice_sim task3.4/lottery
	rm -f task3.5/filecopy task3.6/stack_limit variant15/ulimit_fsize
	rm -f task3.3/dice_results.txt
