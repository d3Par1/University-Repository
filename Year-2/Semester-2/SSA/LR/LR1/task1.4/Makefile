# Завдання 1.4: Багатопоточність (pthreads, OpenMP, C++)
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11
CXXFLAGS = -Wall -Wextra -std=c++17

all: task1_4 task1_4_omp task1_4_cpp

task1_4: main.c module1.c module2.c
	$(CC) $(CFLAGS) -pthread -o $@ $^

task1_4_omp: main.c module1.c module2.c
	$(CC) $(CFLAGS) -fopenmp -pthread -o $@ $^

task1_4_cpp: threaded_cpp.cpp
	$(CXX) $(CXXFLAGS) -pthread -o $@ $<

task1_4_prof: main.c module1.c module2.c
	$(CC) $(CFLAGS) -pg -pthread -o $@ $^

profile: task1_4_prof
	./task1_4_prof
	gprof task1_4_prof gmon.out > profile_report.txt
	@echo "Profile saved to profile_report.txt"

valgrind: task1_4
	valgrind --tool=callgrind ./task1_4

clean:
	rm -f task1_4 task1_4_omp task1_4_cpp task1_4_prof *.o gmon.out callgrind.out.* profile_report.txt

.PHONY: all profile valgrind clean
