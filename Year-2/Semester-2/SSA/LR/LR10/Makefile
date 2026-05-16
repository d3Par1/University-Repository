CC     := gcc
CFLAGS := -Wall -Wextra -O2

TASKS  := task10.1/global_after_fork \
          task10.2/pid_ppid          \
          task10.3/exit_code         \
          task10.4/loop_5_children   \
          task10.5/execlp_ls         \
          task10.6/process_tree      \
          task10.7/sigchld_handler   \
          variant15/wait_order

.PHONY: all clean
all: $(TASKS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TASKS)
