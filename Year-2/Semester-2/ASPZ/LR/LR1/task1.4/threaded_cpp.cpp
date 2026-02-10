// Завдання 1.4: C++ std::thread version
#include <iostream>
#include <thread>
#include <vector>
#include <numeric>
#include <cstdlib>

#define NUM_THREADS 4
#define DEFAULT_N 10000000

struct ThreadArg {
    int start;
    int end;
    double result;
};

void sum_worker(ThreadArg &arg) {
    arg.result = 0.0;
    for (int i = arg.start; i < arg.end; i++) {
        arg.result += 1.0 / (i + 1);
    }
}

int main(int argc, char *argv[]) {
    int n = DEFAULT_N;
    if (argc > 1) n = std::atoi(argv[1]);

    std::cout << "=== Task 1.4: C++ std::thread Demo ===" << std::endl;
    std::cout << "Computing harmonic sum with " << NUM_THREADS
              << " threads, n=" << n << std::endl;

    std::vector<ThreadArg> args(NUM_THREADS);
    std::vector<std::thread> threads;
    int chunk = n / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].start = i * chunk;
        args[i].end = (i == NUM_THREADS - 1) ? n : (i + 1) * chunk;
        threads.emplace_back(sum_worker, std::ref(args[i]));
    }

    double total = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
        total += args[i].result;
    }

    std::cout << "[std::thread] Harmonic sum = "
              << std::fixed << total << std::endl;
    std::cout << "Done." << std::endl;
    return 0;
}
