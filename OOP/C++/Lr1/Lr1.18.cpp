#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <iomanip>

// Константи для хеш-таблиці
#define NHASH 101    // Розмір хеш-таблиці
#define MULTIPLIER 31 // Множник для хеш-функції

// Структура для елемента хеш-таблиці
typedef struct Nameval {
    char *name;             // Ключ
    int value;              // Значення
    struct Nameval *next;   // Наступний елемент у ланцюжку
} Nameval;

// Глобальний масив ланцюжків хеш-таблиці
Nameval *symtab[NHASH];

// Функція для обчислення хеш-значення
unsigned int hash(char *str) {
    unsigned int h = 0;
    unsigned char *p;
    
    for (p = (unsigned char *)str; *p != '\0'; p++) {
        h = MULTIPLIER * h + *p;
    }
    
    return h % NHASH;
}

// Функція для пошуку елемента в хеш-таблиці
Nameval* lookup(char *name) {
    unsigned int h = hash(name);
    Nameval *sym;
    
    for (sym = symtab[h]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            return sym;
        }
    }
    
    return NULL; // Не знайдено
}

// Функція для додавання елемента в хеш-таблицю
Nameval* install(char *name, int value) {
    unsigned int h = hash(name);
    Nameval *sym;
    
    // Перевіряємо, чи ключ вже існує
    sym = lookup(name);
    if (sym != NULL) {
        // Оновлюємо значення
        sym->value = value;
        return sym;
    }
    
    // Створюємо новий елемент
    sym = (Nameval*)malloc(sizeof(Nameval));
    if (sym == NULL) {
        return NULL; // Невдалося виділити пам'ять
    }
    
    sym->name = strdup(name);
    if (sym->name == NULL) {
        free(sym);
        return NULL;
    }
    
    sym->value = value;
    
    // Додаємо на початок ланцюжка
    sym->next = symtab[h];
    symtab[h] = sym;
    
    return sym;
}

// Функція для звільнення пам'яті хеш-таблиці
void free_symtab() {
    for (int i = 0; i < NHASH; i++) {
        Nameval *sym = symtab[i];
        while (sym != NULL) {
            Nameval *next = sym->next;
            free(sym->name);
            free(sym);
            sym = next;
        }
        symtab[i] = NULL;
    }
}

// Функція для аналізу завантаженості хеш-таблиці
void analyze_hash_table() {
    int total_items = 0;
    int empty_buckets = 0;
    int max_chain_length = 0;
    std::vector<int> chain_lengths(NHASH, 0);
    
    // Підрахунок елементів і довжин ланцюжків
    for (int i = 0; i < NHASH; i++) {
        int length = 0;
        Nameval *sym;
        
        for (sym = symtab[i]; sym != NULL; sym = sym->next) {
            length++;
        }
        
        chain_lengths[i] = length;
        total_items += length;
        
        if (length == 0) {
            empty_buckets++;
        }
        
        if (length > max_chain_length) {
            max_chain_length = length;
        }
    }
    
    // Обчислення статистики
    double avg_chain_length = (double)total_items / NHASH;
    double load_factor = (double)total_items / NHASH;
    double occupancy_rate = (double)(NHASH - empty_buckets) / NHASH * 100.0;
    
    // Розподіл довжин ланцюжків
    std::vector<int> distribution(max_chain_length + 1, 0);
    for (int length : chain_lengths) {
        distribution[length]++;
    }
    
    // Виведення результатів
    std::cout << "Аналіз хеш-таблиці:" << std::endl;
    std::cout << "  Розмір таблиці (NHASH): " << NHASH << std::endl;
    std::cout << "  Загальна кількість елементів: " << total_items << std::endl;
    std::cout << "  Коефіцієнт завантаження (load factor): " << load_factor << std::endl;
    std::cout << "  Заповнені комірки: " << (NHASH - empty_buckets) << " з " << NHASH 
              << " (" << std::fixed << std::setprecision(2) << occupancy_rate << "%)" << std::endl;
    std::cout << "  Порожні комірки: " << empty_buckets << std::endl;
    std::cout << "  Середня довжина ланцюжка: " << avg_chain_length << std::endl;
    std::cout << "  Максимальна довжина ланцюжка: " << max_chain_length << std::endl;
    
    std::cout << "\nРозподіл довжин ланцюжків:" << std::endl;
    for (size_t i = 0; i <= std::min(10, max_chain_length); i++) {
        std::cout << "  Довжина " << i << ": " << distribution[i] << " ланцюжків" << std::endl;
    }
    
    if (max_chain_length > 10) {
        std::cout << "  ..." << std::endl;
        std::cout << "  Довжина " << max_chain_length << ": " << distribution[max_chain_length] << " ланцюжків" << std::endl;
    }
}

// Функція для тестування хеш-таблиці з різними розмірами та навантаженнями
void test_hash_table_sizes() {
    const int small_size = 11;    // Малий розмір
    const int medium_size = 101;  // Середній розмір
    const int large_size = 1009;  // Великий розмір
    
    std::cout << "\nДослідження оптимального розміру хеш-таблиці:" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    // Підготовка теоретичної інформації
    std::cout << "Теоретичний аналіз:" << std::endl;
    std::cout << "  Найважливіший фактор для продуктивності хеш-таблиці - це коефіцієнт завантаження (α)" << std::endl;
    std::cout << "  α = n / m, де n - кількість елементів, m - розмір таблиці" << std::endl;
    std::cout << "  Оптимальний коефіцієнт завантаження: α ≈ 0.7" << std::endl;
    std::cout << "  При α < 0.7: витрачається зайва пам'ять" << std::endl;
    std::cout << "  При α > 0.7: збільшується ймовірність колізій і довжина ланцюжків" << std::endl;
    
    // Очікувані довжини ланцюжків для різних коефіцієнтів завантаження
    std::cout << "\nОчікувана середня довжина ланцюжка для різних коефіцієнтів завантаження:" << std::endl;
    std::cout << "  α = 0.5: ~1.5 перевірок на пошук" << std::endl;
    std::cout << "  α = 0.7: ~1.7 перевірок на пошук" << std::endl;
    std::cout << "  α = 1.0: ~2.0 перевірок на пошук" << std::endl;
    std::cout << "  α = 2.0: ~3.0 перевірок на пошук" << std::endl;
    
    // Рекомендації щодо вибору розміру
    std::cout << "\nРекомендації для вибору розміру хеш-таблиці:" << std::endl;
    std::cout << "  1. Розмір таблиці повинен бути простим числом, щоб мінімізувати колізії" << std::endl;
    std::cout << "  2. Для n елементів розмір таблиці повинен бути приблизно m ≈ n / 0.7" << std::endl;
    std::cout << "  3. Для динамічних даних слід передбачити запас, враховуючи майбутнє зростання" << std::endl;
    
    // Розрахунок оптимального розміру для різних наборів даних
    std::cout << "\nРекомендовані розміри для різних кількостей елементів:" << std::endl;
    
    for (int elements : {10, 50, 100, 500, 1000, 5000, 10000}) {
        double optimal_size = elements / 0.7;
        // Знаходимо найближче просте число
        int recommended_size = ceil(optimal_size);
        
        // Це спрощений пошук простого числа, у реальності слід використовувати
        // таблицю відомих простих чисел або більш ефективний алгоритм
        while (true) {
            bool is_prime = true;
            for (int i = 2; i <= sqrt(recommended_size); i++) {
                if (recommended_size % i == 0) {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime) break;
            recommended_size++;
        }
        
        std::cout << "  Для " << elements << " елементів: приблизно " << recommended_size << std::endl;
    }
    
    // Аналіз для прикладу, наведеного в лекції
    std::cout << "\nАналіз для прикладу з NHASH=" << NHASH << ":" << std::endl;
    std::cout << "  Оптимальна кількість елементів: ~" << (int)(NHASH * 0.7) << std::endl;
    std::cout << "  При такій кількості елементів очікувана середня довжина ланцюжка: ~1.7" << std::endl;
    std::cout << "  Якщо кількість елементів значно перевищує це значення," << std::endl;
    std::cout << "  слід розглянути збільшення розміру таблиці або реалізацію реструктуризації." << std::endl;
}

int main() {
    // Ініціалізація хеш-таблиці
    for (int i = 0; i < NHASH; i++) {
        symtab[i] = NULL;
    }
    
    // Додавання тестових даних (50 елементів)
    for (int i = 0; i < 50; i++) {
        char buffer[20];
        sprintf(buffer, "key%d", i);
        install(buffer, i * 10);
    }
    
    // Аналіз хеш-таблиці
    analyze_hash_table();
    
    // Додавання ще даних (до 100 елементів)
    for (int i = 50; i < 100; i++) {
        char buffer[20];
        sprintf(buffer, "key%d", i);
        install(buffer, i * 10);
    }
    
    std::cout << "\nПісля додавання більшої кількості елементів:" << std::endl;
    analyze_hash_table();
    
    // Дослідження оптимального розміру
    test_hash_table_sizes();
    
    // Звільнення пам'яті
    free_symtab();
    
    return 0;
}