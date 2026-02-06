#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <algorithm>

void experiment_uniform_distribution();
void experiment_large_stream();

int main() {
  std::setlocale(LC_ALL, "");
  
  experiment_uniform_distribution();
  experiment_large_stream();
  
  return 0;
}

std::mt19937 rng(std::random_device{}());

template <typename T>
std::vector<T> reservoir_sampling(const std::vector<T>& stream, int k = 1) {
    std::vector<T> result(k);
    
    for (size_t i = 0; i < stream.size(); ++i) {
        if (i < k) {
            result[i] = stream[i];
        } else {
            std::uniform_int_distribution<size_t> dist(0, i);
            size_t j = dist(rng);
            if (j < k) {
                result[j] = stream[i];
            }
        }
    }
    
    return result;
}

template <typename T>
std::vector<T> naive_random_choice(const std::vector<T>& stream, int k = 1) {
    std::vector<T> result(k);
    for (int i = 0; i < k; ++i) {
        std::uniform_int_distribution<size_t> dist(0, stream.size() - 1);
        result[i] = stream[dist(rng)];
    }
    return result;
}

template <typename T>
std::unordered_map<T, int> test_distribution(
    std::vector<T> (*algorithm)(const std::vector<T>&, int),
    const std::vector<T>& stream,
    int num_trials,
    int k = 1
) {
    std::unordered_map<T, int> distribution;
    
    for (int i = 0; i < num_trials; ++i) {
        std::vector<T> chosen = algorithm(stream, k);
        for (const T& item : chosen) {
            distribution[item]++;
        }
    }
    
    return distribution;
}

void experiment_uniform_distribution() {
    std::cout << "Експеримент 1: Перевірка рівномірного розподілу" << std::endl;

    std::vector<int> elements(10);
    for (int i = 0; i < 10; ++i) {
        elements[i] = i + 1;
    }
    
    int num_trials = 10000;

    auto reservoir_dist = test_distribution(reservoir_sampling, elements, num_trials);

    auto naive_dist = test_distribution(naive_random_choice, elements, num_trials);

    double expected = static_cast<double>(num_trials) / elements.size();

    double reservoir_error = 0.0;
    for (const auto& pair : reservoir_dist) {
        reservoir_error += std::abs(pair.second - expected);
    }
    reservoir_error /= (expected * elements.size());

    double naive_error = 0.0;
    for (const auto& pair : naive_dist) {
        naive_error += std::abs(pair.second - expected);
    }
    naive_error /= (expected * elements.size());
    
    std::cout << "Резервуарна вибірка - відхилення від рівномірного розподілу: " 
              << std::fixed << std::setprecision(4) << reservoir_error << std::endl;
    std::cout << "Наївна вибірка - відхилення від рівномірного розподілу: " 
              << std::fixed << std::setprecision(4) << naive_error << std::endl;

    std::ofstream reservoir_file("reservoir_distribution.csv");
    reservoir_file << "Element,Frequency,Expected\n";
    for (int i = 1; i <= 10; ++i) {
        reservoir_file << i << "," << reservoir_dist[i] << "," << expected << "\n";
    }
    reservoir_file.close();
    
    std::ofstream naive_file("naive_distribution.csv");
    naive_file << "Element,Frequency,Expected\n";
    for (int i = 1; i <= 10; ++i) {
        naive_file << i << "," << naive_dist[i] << "," << expected << "\n";
    }
    naive_file.close();
    
    std::cout << "Дані розподілу збережено у файлах 'reservoir_distribution.csv' та 'naive_distribution.csv'" << std::endl;
    std::cout << "Для візуалізації використовуйте зовнішні інструменти (наприклад, Excel або Python з matplotlib)" << std::endl;
}

void experiment_large_stream() {
    std::cout << "\nЕксперимент 2: Тестування для великого потоку даних" << std::endl;

    std::vector<int> stream_sizes = {1000, 10000, 100000, 1000000};
    std::vector<double> reservoir_times;
    std::vector<double> naive_times;
    
    for (int size : stream_sizes) {
        std::vector<int> large_stream(size);
        for (int i = 0; i < size; ++i) {
            large_stream[i] = i;
        }

        auto start = std::chrono::high_resolution_clock::now();
        reservoir_sampling(large_stream);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> reservoir_time = end - start;
        reservoir_times.push_back(reservoir_time.count());

        start = std::chrono::high_resolution_clock::now();
        naive_random_choice(large_stream);
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> naive_time = end - start;
        naive_times.push_back(naive_time.count());
        
        std::cout << "Розмір потоку: " << size << std::endl;
        std::cout << "  Час резервуарної вибірки: " << std::fixed << std::setprecision(6) 
                  << reservoir_time.count() << " сек" << std::endl;
        std::cout << "  Час наївної вибірки: " << std::fixed << std::setprecision(6) 
                  << naive_time.count() << " сек" << std::endl;
    }
    
    std::ofstream time_file("time_comparison.csv");
    time_file << "Size,Reservoir,Naive\n";
    for (size_t i = 0; i < stream_sizes.size(); ++i) {
        time_file << stream_sizes[i] << "," << reservoir_times[i] << "," << naive_times[i] << "\n";
    }
    time_file.close();
    
    std::cout << "Дані часу виконання збережено у файлі 'time_comparison.csv'" << std::endl;
    std::cout << "Для візуалізації використовуйте зовнішні інструменти (наприклад, Excel або Python з matplotlib)" << std::endl;
}

