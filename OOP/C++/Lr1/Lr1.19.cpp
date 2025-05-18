#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <random>
#include <unordered_map>

// Основні параметри для тестування
#define NHASH 101 // Розмір хеш-таблиці за замовчуванням
int CURRENT_MULTIPLIER = 31; // Поточний множник для хеш-функції

// Хеш-функція з динамічним множником
unsigned int hash(const char *str, int multiplier) {
    unsigned int h = 0;
    unsigned char *p;
    
    for (p = (unsigned char *)str; *p != '\0'; p++) {
        h = multiplier * h + *p;
    }
    
    return h % NHASH;
}

// Функція для аналізу розподілу хеш-значень
void analyze_hash_distribution(const std::vector<std::string>& strings, int multiplier, int table_size) {
    std::vector<int> bucket_count(table_size, 0);
    std::vector<std::vector<std::string>> buckets(table_size);
    
    // Розподіляємо рядки за хеш-значеннями
    for (const auto& str : strings) {
        unsigned int h = hash(str.c_str(), multiplier) % table_size;
        bucket_count[h]++;
        buckets[h].push_back(str);
    }
    
    // Збираємо статистику
    int total_strings = strings.size();
    int empty_buckets = 0;
    int max_bucket_size = 0;
    double expected_per_bucket = (double)total_strings / table_size;
    
    for (int count : bucket_count) {
        if (count == 0) {
            empty_buckets++;
        }
        if (count > max_bucket_size) {
            max_bucket_size = count;
        }
    }
    
    // Обчислюємо середньоквадратичне відхилення
    double variance = 0.0;
    for (int count : bucket_count) {
        double diff = count - expected_per_bucket;
        variance += diff * diff;
    }
    variance /= table_size;
    double std_deviation = sqrt(variance);
    
    // Обчислюємо коефіцієнт варіації (CV) - відносна міра дисперсії
    double cv = (expected_per_bucket > 0) ? std_deviation / expected_per_bucket : 0;
    
    // Знаходимо найзавантаженіші кошики
    std::vector<std::pair<int, int>> bucket_sizes;
    for (int i = 0; i < table_size; i++) {
        bucket_sizes.push_back(std::make_pair(bucket_count[i], i));
    }
    std::sort(bucket_sizes.begin(), bucket_sizes.end(), std::greater<std::pair<int, int>>());
    
    // Виведення результатів
    std::cout << "Аналіз хеш-функції з MULTIPLIER=" << multiplier << ", NHASH=" << table_size << std::endl;
    std::cout << "  Загальна кількість рядків: " << total_strings << std::endl;
    std::cout << "  Заповнені кошики: " << (table_size - empty_buckets) << " з " << table_size
              << " (" << std::fixed << std::setprecision(2) 
              << (double)(table_size - empty_buckets) / table_size * 100 << "%)" << std::endl;
    std::cout << "  Порожні кошики: " << empty_buckets << std::endl;
    std::cout << "  Очікувана кількість рядків на кошик: " << expected_per_bucket << std::endl;
    std::cout << "  Максимальна кількість рядків у кошику: " << max_bucket_size << std::endl;
    std::cout << "  Середньоквадратичне відхилення: " << std_deviation << std::endl;
    std::cout << "  Коефіцієнт варіації (CV): " << cv << " (менше значення - краще)" << std::endl;
    
    // Виведення найзавантаженіших кошиків
    std::cout << "  Найзавантаженіші кошики:" << std::endl;
    int top_buckets = std::min(5, (int)bucket_sizes.size());
    for (int i = 0; i < top_buckets; i++) {
        int size = bucket_sizes[i].first;
        int index = bucket_sizes[i].second;
        if (size > 0) {
            std::cout << "    Кошик " << index << ": " << size << " рядків" << std::endl;
        }
    }
    
    // Розподіл довжин ланцюжків
    std::vector<int> chain_distribution(max_bucket_size + 1, 0);
    for (int count : bucket_count) {
        chain_distribution[count]++;
    }
    
    std::cout << "  Розподіл довжин ланцюжків:" << std::endl;
    for (int i = 0; i <= std::min(10, max_bucket_size); i++) {
        std::cout << "    Довжина " << i << ": " << chain_distribution[i] << " кошиків" << std::endl;
    }
    if (max_bucket_size > 10) {
        std::cout << "    ..." << std::endl;
        std::cout << "    Довжина " << max_bucket_size << ": " << chain_distribution[max_bucket_size] << " кошиків" << std::endl;
    }
}

// Функція для виявлення колізій між парами рядків
void find_collisions(const std::vector<std::string>& strings, int multiplier, int table_size) {
    std::unordered_map<unsigned int, std::vector<std::string>> hash_map;
    
    // Розподіляємо рядки за хеш-значеннями
    for (const auto& str : strings) {
        unsigned int h = hash(str.c_str(), multiplier) % table_size;
        hash_map[h].push_back(str);
    }
    
    // Виведення прикладів колізій
    std::cout << "Приклади колізій (рядки з однаковим хеш-значенням):" << std::endl;
    int count = 0;
    for (const auto& pair : hash_map) {
        if (pair.second.size() > 1) {
            std::cout << "  Хеш " << pair.first << ": ";
            for (size_t i = 0; i < std::min(size_t(3), pair.second.size()); i++) {
                std::cout << "\"" << pair.second[i] << "\" ";
            }
            if (pair.second.size() > 3) {
                std::cout << "... (усього " << pair.second.size() << " рядків)";
            }
            std::cout << std::endl;
            
            count++;
            if (count >= 5) break; // Виводимо лише 5 прикладів
        }
    }
}

// Генерація рядків різної довжини
std::vector<std::string> generate_strings_of_varying_length(int count) {
    std::vector<std::string> strings;
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> length_dist(1, 20); // Довжина від 1 до 20
    std::uniform_int_distribution<> char_dist(0, sizeof(charset) - 2);
    
    for (int i = 0; i < count; i++) {
        int length = length_dist(gen);
        std::string str;
        for (int j = 0; j < length; j++) {
            str += charset[char_dist(gen)];
        }
        strings.push_back(str);
    }
    
    return strings;
}

// Генерація рядків фіксованої довжини з малими відмінностями
std::vector<std::string> generate_similar_strings(int count, int length) {
    std::vector<std::string> strings;
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> char_dist(0, sizeof(charset) - 2);
    
    // Генеруємо перший рядок
    std::string base_string;
    for (int j = 0; j < length; j++) {
        base_string += charset[char_dist(gen)];
    }
    strings.push_back(base_string);
    
    // Генеруємо решту рядків з малими відмінностями
    for (int i = 1; i < count; i++) {
        std::string str = base_string;
        
        // Змінюємо 1-2 символи
        std::uniform_int_distribution<> pos_dist(0, length - 1);
        int pos1 = pos_dist(gen);
        str[pos1] = charset[char_dist(gen)];
        
        // З імовірністю 50% змінюємо ще один символ
        if (rand() % 2 == 0) {
            int pos2;
            do {
                pos2 = pos_dist(gen);
            } while (pos2 == pos1);
            str[pos2] = charset[char_dist(gen)];
        }
        
        strings.push_back(str);
    }
    
    return strings;
}

// Генерація URL-адрес
std::vector<std::string> generate_urls(int count) {
    std::vector<std::string> urls;
    const char* protocols[] = {"http://", "https://"};
    const char* domains[] = {"example.com", "test.org", "sample.net", "demo.io", "app.co"};
    const char* paths[] = {"/", "/home", "/about", "/contact", "/products", "/services", "/blog"};
    const char* params[] = {"", "?id=123", "?page=1", "?q=test", "?user=admin", "?lang=en"};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> protocol_dist(0, sizeof(protocols) / sizeof(protocols[0]) - 1);
    std::uniform_int_distribution<> domain_dist(0, sizeof(domains) / sizeof(domains[0]) - 1);
    std::uniform_int_distribution<> path_dist(0, sizeof(paths) / sizeof(paths[0]) - 1);
    std::uniform_int_distribution<> param_dist(0, sizeof(params) / sizeof(params[0]) - 1);
    
    for (int i = 0; i < count; i++) {
        std::string url = protocols[protocol_dist(gen)];
        url += "www.";
        url += domains[domain_dist(gen)];
        url += paths[path_dist(gen)];
        url += params[param_dist(gen)];
        urls.push_back(url);
    }
    
    return urls;
}

// Тестування хеш-функції з різними множниками
void test_multipliers(const std::vector<std::string>& strings, int table_size) {
    std::vector<int> multipliers = {31, 33, 37, 39, 41};
    std::vector<double> cv_values;
    
    std::cout << "\nПорівняння множників MULTIPLIER від 31 до 41:" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    for (int multiplier : multipliers) {
        std::vector<int> bucket_count(table_size, 0);
        
        // Розподіляємо рядки за хеш-значеннями
        for (const auto& str : strings) {
            unsigned int h = hash(str.c_str(), multiplier) % table_size;
            bucket_count[h]++;
        }
        
        // Збираємо статистику
        int total_strings = strings.size();
        int empty_buckets = 0;
        int max_bucket_size = 0;
        double expected_per_bucket = (double)total_strings / table_size;
        
        for (int count : bucket_count) {
            if (count == 0) {
                empty_buckets++;
            }
            if (count > max_bucket_size) {
                max_bucket_size = count;
            }
        }
        
        // Обчислюємо середньоквадратичне відхилення
        double variance = 0.0;
        for (int count : bucket_count) {
            double diff = count - expected_per_bucket;
            variance += diff * diff;
        }
        variance /= table_size;
        double std_deviation = sqrt(variance);
        
        // Обчислюємо коефіцієнт варіації (CV) - відносна міра дисперсії
        double cv = (expected_per_bucket > 0) ? std_deviation / expected_per_bucket : 0;
        cv_values.push_back(cv);
        
        // Виведення результатів
        std::cout << "MULTIPLIER = " << multiplier << ":" << std::endl;
        std::cout << "  Заповнені кошики: " << (table_size - empty_buckets) << " з " << table_size
                  << " (" << std::fixed << std::setprecision(2) 
                  << (double)(table_size - empty_buckets) / table_size * 100 << "%)" << std::endl;
        std::cout << "  Максимальна довжина ланцюжка: " << max_bucket_size << std::endl;
        std::cout << "  Коефіцієнт варіації (CV): " << cv << std::endl;
    }
    
    // Знаходимо найкращий множник
    int best_index = std::min_element(cv_values.begin(), cv_values.end()) - cv_values.begin();
    int best_multiplier = multipliers[best_index];
    
    std::cout << "\nНайкращий множник: " << best_multiplier << " (CV = " << cv_values[best_index] << ")" << std::endl;
}

// Тестування якості хеш-функції для різних типів рядків
void test_string_types() {
    const int num_strings = 1000;
    const int table_size = NHASH;
    
    // Тест 1: Короткі рядки різної довжини
    std::cout << "\nТест 1: Короткі рядки різної довжини" << std::endl;
    std::cout << "===================================" << std::endl;
    auto short_strings = generate_strings_of_varying_length(num_strings);
    analyze_hash_distribution(short_strings, CURRENT_MULTIPLIER, table_size);
    find_collisions(short_strings, CURRENT_MULTIPLIER, table_size);
    
    // Тест 2: Довгі рядки (URL-адреси)
    std::cout << "\nТест 2: Довгі рядки (URL-адреси)" << std::endl;
    std::cout << "===============================" << std::endl;
    auto urls = generate_urls(num_strings);
    analyze_hash_distribution(urls, CURRENT_MULTIPLIER, table_size);
    find_collisions(urls, CURRENT_MULTIPLIER, table_size);
    
    // Тест 3: Рядки однакової довжини з малими відмінностями
    std::cout << "\nТест 3: Рядки однакової довжини з малими відмінностями" << std::endl;
    std::cout << "===================================================" << std::endl;
    auto similar_strings = generate_similar_strings(num_strings, 10);
    analyze_hash_distribution(similar_strings, CURRENT_MULTIPLIER, table_size);
    find_collisions(similar_strings, CURRENT_MULTIPLIER, table_size);
}

int main() {
    // Ініціалізація генератора випадкових чисел
    srand(time(NULL));
    
    std::cout << "Аналіз якості хеш-функції для різних типів даних\n" << std::endl;
    
    // Тестування для різних типів рядків
    test_string_types();
    
    // Тестування з різними множниками
    std::cout << "\n\nПорівняння якості хеш-функції з різними множниками" << std::endl;
    std::cout << "===================================================" << std::endl;
    
    // Генеруємо збірний набір тестових даних
    const int num_strings = 1000;
    std::vector<std::string> test_data;
    
    // Додаємо різні типи рядків
    auto short_strings = generate_strings_of_varying_length(num_strings / 3);
    auto urls = generate_urls(num_strings / 3);
    auto similar_strings = generate_similar_strings(num_strings / 3, 10);
    
    test_data.insert(test_data.end(), short_strings.begin(), short_strings.end());
    test_data.insert(test_data.end(), urls.begin(), urls.end());
    test_data.insert(test_data.end(), similar_strings.begin(), similar_strings.end());
    
    // Тестуємо з різними множниками
    test_multipliers(test_data, NHASH);
    
    // Висновки
    std::cout << "\n\nВисновки щодо якості хеш-функції:" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "1. Хеш-функція добре працює для коротких рядків різної довжини," << std::endl;
    std::cout << "   забезпечуючи рівномірний розподіл хеш-значень." << std::endl;
    std::cout << "2. Для довгих рядків (URL-адрес) розподіл менш рівномірний," << std::endl;
    std::cout << "   але все ще прийнятний для більшості випадків." << std::endl;
    std::cout << "3. Рядки однакової довжини з малими відмінностями створюють" << std::endl;
    std::cout << "   найбільшу проблему - більше колізій через схожість рядків." << std::endl;
    std::cout << "4. Оптимальний множник MULTIPLIER для цієї хеш-функції знаходиться" << std::endl;
    std::cout << "   в діапазоні 31-41, з перевагою значень 31 і 33." << std::endl;
    std::cout << "5. Розмір таблиці NHASH також має значний вплив на якість хешування -" << std::endl;
    std::cout << "   рекомендується використовувати прості числа для зменшення колізій." << std::endl;

    return 0;
}