#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <chrono>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <numeric>
#include <sstream>

int main() {
  std::setlocale(LC_ALL, "");
  
  std::vector<std::string> file_paths = {
      "sample1.txt",
      "sample2.txt",
      "hemingway.txt"
  };
  
  bool create_files = false;
  for (const auto& path : file_paths) {
      std::ifstream f(path);
      if (!f.good()) {
          create_files = true;
          break;
      }
  }
  
  if (create_files) {
      create_test_files();
  }
  
  std::vector<std::string> existing_files;
  for (const auto& path : file_paths) {
      std::ifstream f(path);
      if (f.good()) {
          existing_files.push_back(path);
      }
  }
  
  auto results = (compare_approaches(existing_files));
  
  if (!results.empty()) {
      (save_results_to_csv(results));
  }
  
  return 0;
}

struct ProcessingStats {
    size_t total_words = 0;
    size_t unique_words = 0;
    size_t memory_usage = 0;
    double processing_time = 0.0;
    double memory_profile = 0.0;
};

struct ComparisonResult {
    std::string file_name;
    size_t file_size;
    size_t total_words;
    size_t unique_words;
    double uniqueness_ratio;
    double standard_memory;
    double dedup_memory;
    double memory_saving;
    double standard_time;
    double dedup_time;
    double time_diff;
};

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

class TextProcessor {
private:
    std::unordered_map<std::pair<std::string, std::string>, std::vector<std::string>, 
        PairHash> prefix_table;
    
    std::unordered_map<std::string, size_t> word_table;
    
    bool use_deduplication;
    ProcessingStats stats;
 
    std::string get_word_id(const std::string& word) {
        if (!use_deduplication) {
            return word;
        }
        
        if (word_table.find(word) == word_table.end()) {
            word_table[word] = word_table.size();
            stats.unique_words++;
        }
        
        return std::to_string(word_table[word]);
    }
    
public:
    TextProcessor(bool use_dedup = false) : use_deduplication(use_dedup) {}
    
    void process_text(const std::string& text) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<std::string> words;
        std::regex word_regex("\\w+");
        
        auto words_begin = std::sregex_iterator(text.begin(), text.end(), word_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::string word = i->str();
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            words.push_back(word);
        }
        
        stats.total_words = words.size();
        
        if (words.size() < 2) {
            auto end_time = std::chrono::high_resolution_clock::now();
            stats.processing_time = std::chrono::duration<double>(end_time - start_time).count();
            return;
        }
        
        for (size_t i = 0; i < words.size() - 2; ++i) {
            std::pair<std::string, std::string> prefix(
                get_word_id(words[i]), 
                get_word_id(words[i + 1])
            );
            std::string suffix = get_word_id(words[i + 2]);
            
            prefix_table[prefix].push_back(suffix);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        stats.processing_time = std::chrono::duration<double>(end_time - start_time).count();
    }
    
    size_t estimate_memory_usage() const {
        size_t size = 0;
        
        for (const auto& [key, value] : prefix_table) {
            // Розмір ключа (пари рядків)
            size += key.first.size() + key.second.size() + 2; // +2 для нуль-термінаторів
            
            size += sizeof(value) + value.capacity() * sizeof(std::string);
            
            for (const auto& str : value) {
                size += str.size() + 1; // +1 для нуль-термінатора
            }
        }
        
        if (use_deduplication) {
            for (const auto& [key, value] : word_table) {
                size += key.size() + 1 + sizeof(value); // +1 для нуль-термінатора
            }
        }
        
        return size;
    }
    
    ProcessingStats get_stats() {
        stats.memory_usage = estimate_memory_usage();
        return stats;
    }
};

std::vector<ComparisonResult> compare_approaches(const std::vector<std::string>& file_paths) {
    std::vector<ComparisonResult> results;
    
    for (const auto& file_path : file_paths) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Помилка відкриття файлу: " << file_path << std::endl;
            continue;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string text = buffer.str();
        
        size_t file_size = text.size();
        std::string file_name = file_path.substr(file_path.find_last_of("/\\") + 1);
        
        std::cout << "\nОбробка файлу: " << file_name << " (розмір: " << file_size << " байт)" << std::endl;
        
        TextProcessor standard_processor(false);
        
        auto start_std = std::chrono::high_resolution_clock::now();
        standard_processor.process_text(text);
        auto end_std = std::chrono::high_resolution_clock::now();
        
        ProcessingStats std_stats = standard_processor.get_stats();
        std_stats.memory_profile = static_cast<double>(std_stats.memory_usage) / (1024 * 1024);
        std_stats.processing_time = std::chrono::duration<double>(end_std - start_std).count();
        
        TextProcessor dedup_processor(true);
        
        auto start_dedup = std::chrono::high_resolution_clock::now();
        dedup_processor.process_text(text);
        auto end_dedup = std::chrono::high_resolution_clock::now();
        
        ProcessingStats dedup_stats = dedup_processor.get_stats();
        dedup_stats.memory_profile = static_cast<double>(dedup_stats.memory_usage) / (1024 * 1024);
        dedup_stats.processing_time = std::chrono::duration<double>(end_dedup - start_dedup).count();
        
        double memory_saving = (std_stats.memory_profile - dedup_stats.memory_profile) / std_stats.memory_profile * 100.0;
        double time_diff = (std_stats.processing_time - dedup_stats.processing_time) / std_stats.processing_time * 100.0;
        
        ComparisonResult result = {
            file_name,
            file_size,
            std_stats.total_words,
            dedup_stats.unique_words,
            static_cast<double>(dedup_stats.unique_words) / std_stats.total_words * 100.0,
            std_stats.memory_profile,
            dedup_stats.memory_profile,
            memory_saving,
            std_stats.processing_time,
            dedup_stats.processing_time,
            time_diff
        };
        
        results.push_back(result);
        
        std::cout << "Загальна кількість слів: " << result.total_words << std::endl;
        std::cout << "Унікальних слів: " << result.unique_words << " (" 
                  << std::fixed << std::setprecision(2) << result.uniqueness_ratio << "%)" << std::endl;
        std::cout << "Пам'ять (стандартна): " << std::fixed << std::setprecision(2) << result.standard_memory << " МБ" << std::endl;
        std::cout << "Пам'ять (дедуплікація): " << std::fixed << std::setprecision(2) << result.dedup_memory << " МБ" << std::endl;
        std::cout << "Економія пам'яті: " << std::fixed << std::setprecision(2) << result.memory_saving << "%" << std::endl;
        std::cout << "Час (стандартна): " << std::fixed << std::setprecision(4) << result.standard_time << " сек" << std::endl;
        std::cout << "Час (дедуплікація): " << std::fixed << std::setprecision(4) << result.dedup_time << " сек" << std::endl;
        std::cout << "Різниця в часі: " << std::fixed << std::setprecision(2) << result.time_diff << "%" << std::endl;
    }
    
    return results;
}

void create_test_files() {
    std::cout << "Створення тестових файлів..." << std::endl;
    
    {
        std::ofstream f("sample1.txt");
        f << "Це невеликий тестовий файл для демонстрації роботи алгоритму. "
             "У цьому файлі буде багато повторень слів, тому що повторення "
             "слів дозволяє побачити ефективність дедуплікації. Повторення "
             "слів є ключовим для нашого тесту.";
    }
    
    {
        std::ofstream f("sample2.txt");
        std::vector<std::string> phrases = {
            "Марківський ланцюг - це випадковий процес.",
            "Алгоритми обробки тексту важливі для аналізу даних.",
            "Дедуплікація дозволяє економити пам'ять при обробці тексту.",
            "Хеш-таблиці забезпечують швидкий доступ до даних."
        };
        
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 10; ++j) {
                f << phrases[j % phrases.size()] << " ";
            }
        }
    }
    
    std::cout << "Для третього тесту потрібно вручну завантажити файл hemingway.txt "
              << "з тексту 'Сонце також сходить' за посиланням із завдання." << std::endl;
}

void save_results_to_csv(const std::vector<ComparisonResult>& results) {
    std::ofstream csv_file("hash_table_optimization_results.csv");
    csv_file << "File,Size,TotalWords,UniqueWords,UniquenessRatio,StandardMemory,DedupMemory,"
             << "MemorySaving,StandardTime,DedupTime,TimeDiff\n";
             
    for (const auto& r : results) {
        csv_file << r.file_name << ","
                 << r.file_size << ","
                 << r.total_words << ","
                 << r.unique_words << ","
                 << std::fixed << std::setprecision(2) << r.uniqueness_ratio << ","
                 << std::fixed << std::setprecision(2) << r.standard_memory << ","
                 << std::fixed << std::setprecision(2) << r.dedup_memory << ","
                 << std::fixed << std::setprecision(2) << r.memory_saving << ","
                 << std::fixed << std::setprecision(4) << r.standard_time << ","
                 << std::fixed << std::setprecision(4) << r.dedup_time << ","
                 << std::fixed << std::setprecision(2) << r.time_diff << "\n";
    }
    
    csv_file.close();
    std::cout << "\nРезультати збережено у файлі 'hash_table_optimization_results.csv'" << std::endl;
    std::cout << "Для візуалізації використовуйте зовнішні інструменти (наприклад, Excel або Python з matplotlib)" << std::endl;
}

