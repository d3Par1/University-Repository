CC     := gcc
CFLAGS := -Wall -Wextra -O2

TASKS  := task7.1/popen_who    \
          task7.2/my_ls         \
          task7.3/my_grep       \
          task7.4/my_more       \
          task7.5/recursive_ls  \
          task7.6/subdirs_sorted\
          task7.7/grant_read    \
          task7.8/delete_prompt \
          task7.9/timer_ms      \
          task7.10/random_floats\
          variant15/dir_tree

.PHONY: all clean
all: $(TASKS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TASKS)
