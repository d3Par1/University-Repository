#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <cmath>
#include <iomanip>
#include <chrono>

// Константи для хеш-таблиці
#define MULTIPLIER 31 // Множник для хеш-функції

// Хеш-функція з параметрами
unsigned int hash(const char *str, int multiplier, int nhash) {
    unsigned int h = 0;
    unsigned char *p;
    
    for (p = (unsigned char *)str; *p != '\0'; p++) {
        h = multiplier * h + *p;
    }
    
    return h % nhash;
}

// Генерація "поганого" набору даних, який спричиняє колізії
std::vector<std::string> generate_bad_dataset(int size, int multiplier, int nhash) {
    std::vector<std::string> dataset;
    std::unordered_map<int, std::vector<std::string>> hash_buckets;
    
    // Дозволимо лише 3 хеш-значення для поганого набору даних
    int target_hash1 = rand() % nhash;
    int target_hash2 = (target_hash1 + nhash / 3) % nhash;
    int target_hash3 = (target_hash2 + nhash / 3) % nhash;
    
    std::set<int> target_hashes = {target_hash1, target_hash2, target_hash3};
    
    std::cout << "Генерація поганого набору даних для хеш-функції..." << std::endl;
    std::cout << "Цільові хеш-значення: " << target_hash1 << ", " << target_hash2 << ", " << target_hash3 << std::endl;
    
    // Базовий набір символів для генерації
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    int found = 0;
    int attempts = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Генеруємо рядки, поки не отримаємо достатню кількість з бажаними хеш-значеннями
    while (found < size && attempts < size * 1000) {
        attempts++;
        
        // Генеруємо випадковий рядок
        int len = 5 + rand() % 10; // Довжина від 5 до 14
        std::string str;
        for (int i = 0; i < len; i++) {
            str += charset[rand() % charset.length()];
        }
        
        // Обчислюємо хеш-значення
        unsigned int h = hash(str.c_str(), multiplier, nhash);
        
        // Якщо хеш потрапляє в цільові значення, додаємо рядок до набору
        if (target_hashes.find(h) != target_hashes.end()) {
            hash_buckets[h].push_back(str);
            dataset.push_back(str);
            found++;
            
            // Виводимо прогрес
            if (found % (size / 10 + 1) == 0 || found == size) {
                std::cout << "  Прогрес: " << found << "/" << size 
                          << " (" << std::fixed << std::setprecision(2) 
                          << (double)found / size * 100 << "%)" << std::endl;
            }
        }
        
        // Виводимо прогрес пошуку
        if (attempts % 100000 == 0) {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
            
            std::cout << "  Спроб: " << attempts << ", знайдено: " << found 
                      << ", час: " << duration << " сек" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
    
    // Статистика пошуку
    std::cout << "\nСтатистика генерації:" << std::endl;
    std::cout << "  Загальна кількість спроб: " << attempts << std::endl;
    std::cout << "  Загальний час: " << duration << " секунд" << std::endl;
    std::cout << "  Ефективність: " << std::fixed << std::setprecision(6) 
              << (double)found / attempts * 100 << "%" << std::endl;
    
    // Розподіл рядків за хеш-значеннями
    std::cout << "\nРозподіл рядків за цільовими хеш-значеннями:" << std::endl;
    for (const auto& pair : hash_buckets) {
        std::cout << "  Хеш " << pair.first << ": " << pair.second.size() << " рядків" << std::endl;
    }
    
    return dataset;
}

// Аналіз "поганого" набору даних
void analyze_bad_dataset(const std::vector<std::string>& dataset, int multiplier, int nhash) {
    std::unordered_map<int, int> hash_counts;
    
    // Підрахунок частоти кожного хеш-значення
    for (const auto& str : dataset) {
        unsigned int h = hash(str.c_str(), multiplier, nhash);
        hash_counts[h]++;
    }
    
    // Збираємо статистику
    int total_strings = dataset.size();
    int used_buckets = hash_counts.size();
    int max_bucket_size = 0;
    
    for (const auto& pair : hash_counts) {
        if (pair.second > max_bucket_size) {
            max_bucket_size = pair.second;
        }
    }
    
    // Сортуємо хеш-значення за кількістю елементів
    std::vector<std::pair<int, int>> sorted_hashes;
    for (const auto& pair : hash_counts) {
        sorted_hashes.push_back(pair);
    }
    std::sort(sorted_hashes.begin(), sorted_hashes.end(), 
             [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                 return a.second > b.second;
             });
    
    // Виведення результатів
    std::cout << "\nАналіз \"поганого\" набору даних:" << std::endl;
    std::cout << "  Загальна кількість рядків: " << total_strings << std::endl;
    std::cout << "  Використано кошиків: " << used_buckets << " з " << nhash 
              << " (" << std::fixed << std::setprecision(2) 
              << (double)used_buckets / nhash * 100 << "%)" << std::endl;
    std::cout << "  Максимальна кількість елементів у кошику: " << max_bucket_size << std::endl;
    std::cout << "  Співвідношення макс/середнє: " << std::fixed << std::setprecision(2)
              << (double)max_bucket_size / ((double)total_strings / nhash) << "x" << std::endl;
    
    // Найбільш заповнені кошики
    std::cout << "\nНайбільш заповнені кошики:" << std::endl;
    int top_n = std::min(5, (int)sorted_hashes.size());
    for (int i = 0; i < top_n; i++) {
        std::cout << "  Хеш " << sorted_hashes[i].first << ": " 
                  << sorted_hashes[i].second << " елементів" << std::endl;
    }
}

// Оцінка складності побудови "поганого" набору для різних значень NHASH
void evaluate_difficulty_for_different_nhash() {
    std::vector<int> nhash_values = {17, 101, 1009, 10007};
    const int dataset_size = 100; // Зменшимо для швидшого тестування
    
    std::cout << "\nСкладність побудови \"поганого\" набору для різних значень NHASH" << std::endl;
    std::cout << "============================================================" << std::endl;
    
    for (int nhash : nhash_values) {
        std::cout << "\nТестування для NHASH = " << nhash << std::endl;
        std::cout << "------------------------" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Генеруємо лише невелику кількість елементів для оцінки складності
        std::vector<std::string> small_dataset = generate_bad_dataset(dataset_size, MULTIPLIER, nhash);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        // Аналіз згенерованого набору
        analyze_bad_dataset(small_dataset, MULTIPLIER, nhash);
        
        // Оцінка складності
        double time_per_item = (double)duration / dataset_size;
        
        std::cout << "\nОцінка складності:" << std::endl;
        std::cout << "  Час на генерацію " << dataset_size << " елементів: " 
                  << duration << " мс (" << duration/1000.0 << " сек)" << std::endl;
        std::cout << "  Середній час на елемент: " << time_per_item << " мс" << std::endl;
        
        // Оцінка для більшого набору
        int larger_size = 1000;
        double estimated_time = time_per_item * larger_size / 1000.0; // в секундах
        
        std::cout << "  Оцінка часу для генерації " << larger_size << " елементів: " 
                  << estimated_time << " сек" << std::endl;
    }
}

// Генерація набору рядків, які мають однакове хеш-значення
std::vector<std::string> find_strings_with_same_hash(int target_hash, int multiplier, int nhash, int count) {
    std::vector<std::string> result;
    std::string charset = "abcdefghijklmnopqrstuvwxyz";
    
    int attempts = 0;
    while (result.size() < count && attempts < count * 1000) {
        attempts++;
        
        // Генеруємо випадковий рядок
        int len = 5 + rand() % 10; // Довжина від 5 до 14
        std::string str;
        for (int i = 0; i < len; i++) {
            str += charset[rand() % charset.length()];
        }
        
        // Обчислюємо хеш-значення
        unsigned int h = hash(str.c_str(), multiplier, nhash);
        
        // Якщо хеш збігається з цільовим, додаємо рядок до результату
        if (h == target_hash) {
            result.push_back(str);
        }
    }
    
    return result;
}

// Пошук наборів рядків з однаковим хеш-значенням для різних NHASH
void find_hash_collisions_for_different_nhash() {
    std::vector<int> nhash_values = {17, 101, 1009};
    const int target_count = 5; // Шукаємо 5 рядків з однаковим хеш-значенням
    
    std::cout << "\nПошук рядків з однаковим хеш-значенням для різних NHASH" << std::endl;
    std::cout << "======================================================" << std::endl;
    
    for (int nhash : nhash_values) {
        std::cout << "\nNHASH = " << nhash << ":" << std::endl;
        
        // Вибираємо випадкове хеш-значення
        int target_hash = rand() % nhash;
        
        std::cout << "  Пошук " << target_count << " рядків з хеш-значенням " << target_hash << "..." << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Шукаємо рядки з однаковим хеш-значенням
        std::vector<std::string> colliding_strings = find_strings_with_same_hash(target_hash, MULTIPLIER, nhash, target_count);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        // Виводимо результати
        std::cout << "  Знайдено " << colliding_strings.size() << " рядків за " << duration << " мс:" << std::endl;
        for (const auto& str : colliding_strings) {
            std::cout << "    \"" << str << "\" -> хеш = " << hash(str.c_str(), MULTIPLIER, nhash) << std::endl;
        }
    }
}

int main() {
    // Ініціалізація генератора випадкових чисел
    srand(time(NULL));
    
    std::cout << "Побудова \"поганого\" набору даних для хеш-функції\n" << std::endl;
    
    // Параметри для основного тесту
    const int nhash = 101;
    const int dataset_size = 1000;
    
    // 1. Генерація "поганого" набору даних
    std::vector<std::string> bad_dataset = generate_bad_dataset(dataset_size, MULTIPLIER, nhash);
    
    // 2. Аналіз згенерованого набору
    std::cout << "\nДетальний аналіз згенерованого \"поганого\" набору:" << std::endl;
    analyze_bad_dataset(bad_dataset, MULTIPLIER, nhash);
    
    // 3. Оцінка складності побудови "поганого" набору для різних NHASH
    evaluate_difficulty_for_different_nhash();
    
    // 4. Пошук рядків з однаковим хеш-значенням для різних NHASH
    find_hash_collisions_for_different_nhash();
    
    // Висновки
    std::cout << "\n\nВисновки:" << std::endl;
    std::cout << "=========" << std::endl;
    std::cout << "1. Складність побудови \"поганого\" набору даних значно зростає зі" << std::endl;
    std::cout << "   збільшенням розміру хеш-таблиці (NHASH)." << std::endl;
    std::cout << "2. Для невеликих значень NHASH (наприклад, 17) досить легко знайти" << std::endl;
    std::cout << "   набір рядків, які всі потрапляють у кілька кошиків." << std::endl;
    std::cout << "3. Для великих значень NHASH (наприклад, 10007) побудова набору, де" << std::endl;
    std::cout << "   всі елементи потрапляють у невелику кількість кошиків, стає" << std::endl;
    std::cout << "   практично неможливою задачею (без знання внутрішніх деталей хеш-функції)." << std::endl;
    std::cout << "4. Хеш-функція з множником MULTIPLIER = 31 загалом добре розподіляє" << std::endl;
    std::cout << "   випадкові рядки, але може бути вразливою до спеціально підібраних" << std::endl;
    std::cout << "   вхідних даних." << std::endl;
    std::cout << "5. Вибір NHASH як простого числа суттєво ускладнює завдання побудови" << std::endl;
    std::cout << "   \"поганого\" набору даних." << std::endl;
    
    return 0;
}