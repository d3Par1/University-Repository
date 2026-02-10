CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJS = main.o input.o output.o

# Directories for libraries
SYSLIB_DIR = /usr/lib
USERLIB_DIR = $(HOME)/mylibs

all: setup my_program

# Create user library directory and build libraries
setup:
	@mkdir -p $(USERLIB_DIR)

# --- Static libraries (.a) ---
libprocess1.a: process1.o
	ar rcs $@ $<

libprocess.a: process.o
	ar rcs $@ $<

# --- Dynamic/shared libraries (.so) ---
libprocess1.so: process1.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

libprocess.so: process.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

# --- Object files ---
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Build with static libraries ---
my_program: $(OBJS) libprocess1.a libprocess.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L. -lprocess1 -lprocess

# --- Build with shared libraries ---
my_program_shared: $(OBJS) libprocess1.so libprocess.so
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L. -lprocess1 -lprocess -Wl,-rpath,.

# --- Build with debug info ---
my_program_dbg: CFLAGS += -g -O0
my_program_dbg: $(OBJS) libprocess1.a libprocess.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L. -lprocess1 -lprocess

# --- Build with optimization ---
my_program_opt: CFLAGS += -O2
my_program_opt: $(OBJS) libprocess1.a libprocess.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L. -lprocess1 -lprocess

# --- Install libraries to system/user dirs ---
install_libs: libprocess1.so libprocess.so libprocess1.a libprocess.a
	cp libprocess.so libprocess.a $(USERLIB_DIR)/
	@echo "Libraries installed to $(USERLIB_DIR)"
	@echo "To install to $(SYSLIB_DIR), run: sudo cp libprocess1.so libprocess1.a $(SYSLIB_DIR)/"

# --- Using libraries from different directories ---
my_program_syslib: $(OBJS) install_libs
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L$(SYSLIB_DIR) -lprocess1 -L$(USERLIB_DIR) -lprocess

clean:
	rm -f *.o *.a *.so my_program my_program_shared my_program_dbg my_program_opt my_program_syslib

.PHONY: all setup install_libs clean
