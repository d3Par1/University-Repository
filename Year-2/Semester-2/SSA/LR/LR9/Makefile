CC     := gcc
CFLAGS := -Wall -Wextra -O2

TASKS  := task9.1/users        \
          task9.2/sudo_shadow  \
          task9.3/root_copy    \
          task9.4/whoami_id    \
          task9.5/chmod_check  \
          task9.6/ls_dirs      \
          variant15/cmd_logger

.PHONY: all clean
all: $(TASKS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TASKS)
