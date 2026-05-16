CC     := gcc
CFLAGS := -Wall -Wextra -O2

TASKS  := task11.1/sigsegv_handler   \
          task11.2/sigaction_basics  \
          task11.3/sigqueue_data     \
          task11.4/signalfd_poll     \
          task11.5/sigsuspend_mask   \
          variant15/signal_timeline

.PHONY: all clean
all: $(TASKS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TASKS)
