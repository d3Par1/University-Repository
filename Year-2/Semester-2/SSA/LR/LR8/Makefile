CC     := gcc
CFLAGS := -Wall -Wextra -O2

TASKS  := task8.1/partial_write \
          task8.2/lseek_read    \
          task8.3/qsort_worst   \
          task8.4/fork_pid      \
          variant15/atomic_swap

.PHONY: all clean
all: $(TASKS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TASKS)
