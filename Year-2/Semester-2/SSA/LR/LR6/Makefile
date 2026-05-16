CC      := gcc
CFLAGS  := -Wall -Wextra -g -O0 -no-pie
LDFLAGS :=

VARIANT := variant15
TARGET  := $(VARIANT)/target
DEBUGGER:= $(VARIANT)/debugger

.PHONY: all clean run

all: $(TARGET) $(DEBUGGER)

$(TARGET): $(VARIANT)/target.c
	$(CC) $(CFLAGS) -no-pie -o $@ $<

$(DEBUGGER): $(VARIANT)/debugger.c
	$(CC) $(CFLAGS) -o $@ $<

run: all
	./$(DEBUGGER) ./$(TARGET) target_function

clean:
	rm -f $(TARGET) $(DEBUGGER)
