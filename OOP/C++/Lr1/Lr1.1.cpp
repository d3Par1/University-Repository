#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

// Початкові константи для хеш-таблиці
#define INITIAL_SIZE 11    // Початковий розмір хеш-таблиці
#define MULTIPLIER 31      // Множник для хеш-функції
#define LOAD_THRESHOLD 0.7 // Поріг середньої довжини ланцюжка для розширення
#define GROWTH_FACTOR 2    // Коефіцієнт розширення


// Структура для елемента хеш-таблиці
typedef struct Nameval {
    char *name;             // Ключ
    int value;              // Значення
    struct Nameval *next;   // Наступний елемент у ланцюжку
} Nameval;

// Структура для хеш-таблиці
typedef struct {
    Nameval **table;       // Масив ланцюжків
    int size;              // Поточний розмір таблиці
    int count;             // Кількість елементів у таблиці
    int resizes;           // Кількість операцій розширення таблиці
    unsigned long lookups; // Кількість операцій пошуку
    unsigned long probes;  // Загальна кількість перевірок при пошуку
} HashTable;

// Глобальна хеш-таблиця
HashTable hash_table;

// Функція для обчислення хеш-значення
unsigned int hash(char *str, int table_size) {
    unsigned int h = 0;
    unsigned char *p;
    
    for (p = (unsigned char *)str; *p != '\0'; p++) {
        h = MULTIPLIER * h + *p;
    }
    
    return h % table_size;
}

// Функція для ініціалізації хеш-таблиці
void init_hash_table(int size) {
    hash_table.table = (Nameval **)malloc(size * sizeof(Nameval *));
    if (hash_table.table == NULL) {
        std::cerr << "Помилка: Неможливо виділити пам'ять для хеш-таблиці" << std::endl;
        exit(1);
    }
    
    for (int i = 0; i < size; i++) {
        hash_table.table[i] = NULL;
    }
    
    hash_table.size = size;
    hash_table.count = 0;
    hash_table.resizes = 0;
    hash_table.lookups = 0;
    hash_table.probes = 0;
}

// Функція для обчислення наступного простого числа
bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    
    return true;
}

int next_prime(int n) {
    if (n <= 1) return 2;
    
    int prime = n;
    bool found = false;
    
    while (!found) {
        prime++;
        if (is_prime(prime))
            found = true;
    }
    
    return prime;
}

// Функція для розширення хеш-таблиці
void resize_hash_table() {
    int old_size = hash_table.size;
    Nameval **old_table = hash_table.table;
    
    // Обчислюємо новий розмір (наступне просте число після old_size * GROWTH_FACTOR)
    int new_size = next_prime(old_size * GROWTH_FACTOR);
    
    std::cout << "Розширення хеш-таблиці: " << old_size << " -> " << new_size << std::endl;
    
    // Створюємо нову таблицю
    Nameval **new_table = (Nameval **)malloc(new_size * sizeof(Nameval *));
    if (new_table == NULL) {
        std::cerr << "Помилка: Неможливо виділити пам'ять для нової хеш-таблиці" << std::endl;
        return; // Продовжуємо з поточною таблицею
    }
    
    // Ініціалізуємо нову таблицю
    for (int i = 0; i < new_size; i++) {
        new_table[i] = NULL;
    }
    
    // Переміщуємо всі елементи в нову таблицю
    int total_items = 0;
    for (int i = 0; i < old_size; i++) {
        Nameval *current = old_table[i];
        
        while (current != NULL) {
            Nameval *next = current->next;
            
            // Обчислюємо новий хеш для елемента
            unsigned int new_hash = hash(current->name, new_size);
            
            // Додаємо елемент на початок нового ланцюжка
            current->next = new_table[new_hash];
            new_table[new_hash] = current;
            
            total_items++;
            current = next;
        }
    }
    
    // Оновлюємо структуру хеш-таблиці
    hash_table.table = new_table;
    hash_table.size = new_size;
    hash_table.resizes++;
    
    // Перевіряємо, що всі елементи були перенесені
    if (total_items != hash_table.count) {
        std::cerr << "Помилка: Кількість перенесених елементів (" << total_items
                  << ") не відповідає очікуваній кількості (" << hash_table.count << ")" << std::endl;
    }
    
    // Звільняємо стару таблицю
    free(old_table);
}

// Функція для пошуку елемента в хеш-таблиці з автоматичним розширенням
Nameval* lookup(char *name) {
    hash_table.lookups++;
    
    // Обчислюємо поточну середню довжину ланцюжка
    double avg_chain_length = (double)hash_table.count / hash_table.size;
    
    // Якщо середня довжина ланцюжка перевищує поріг, розширюємо таблицю
    if (avg_chain_length > LOAD_THRESHOLD) {
        resize_hash_table();
    }
    
    // Обчислюємо хеш-значення
    unsigned int h = hash(name, hash_table.size);
    Nameval *sym;
    
    // Шукаємо елемент у ланцюжку
    int probes = 0;
    for (sym = hash_table.table[h]; sym != NULL; sym = sym->next) {
        probes++;
        if (strcmp(name, sym->name) == 0) {
            hash_table.probes += probes;
            return sym;
        }
    }
    
    hash_table.probes += probes;
    return NULL; // Не знайдено
}

// Функція для додавання елемента в хеш-таблицю
Nameval* install(char *name, int value) {
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
    unsigned int h = hash(name, hash_table.size);
    sym->next = hash_table.table[h];
    hash_table.table[h] = sym;
    
    // Збільшуємо лічильник елементів
    hash_table.count++;
    
    return sym;
}

// Функція для звільнення пам'яті хеш-таблиці
void free_hash_table() {
    for (int i = 0; i < hash_table.size; i++) {
        Nameval *sym = hash_table.table[i];
        while (sym != NULL) {
            Nameval *next = sym->next;
            free(sym->name);
            free(sym);
            sym = next;
        }
    }
    
    free(hash_table.table);
    hash_table.table = NULL;
    hash_table.size = 0;
    hash_table.count = 0;
}

// Функція для аналізу хеш-таблиці
void analyze_hash_table() {
    // Підрахунок довжин ланцюжків
    std::vector<int> chain_lengths(hash_table.size, 0);
    for (int i = 0; i < hash_table.size; i++) {
        int length = 0;
        Nameval *sym;
        
        for (sym = hash_table.table[i]; sym != NULL; sym = sym->next) {
            length++;
        }
        
        chain_lengths[i] = length;
    }
    
    // Обчислення статистики
    int empty_buckets = 0;
    int max_chain_length = 0;
    double avg_chain_length = (double)hash_table.count / hash_table.size;
    double avg_probes = (hash_table.lookups > 0) ? (double)hash_table.probes / hash_table.lookups : 0;
    
    for (int length : chain_lengths) {
        if (length == 0) {
            empty_buckets++;
        }
        if (length > max_chain_length) {
            max_chain_length = length;
        }
    }
    
    // Розподіл довжин ланцюжків
    std::vector<int> distribution(max_chain_length + 1, 0);
    for (int length : chain_lengths) {
        distribution[length]++;
    }
    
    // Виведення результатів
    std::cout << "Аналіз хеш-таблиці:" << std::endl;
    std::cout << "  Розмір таблиці: " << hash_table.size << std::endl;
    std::cout << "  Кількість елементів: " << hash_table.count << std::endl;
    std::cout << "  Коефіцієнт завантаження (load factor): " 
              << std::fixed << std::setprecision(2) << avg_chain_length << std::endl;
    std::cout << "  Заповнені кошики: " << (hash_table.size - empty_buckets) << " з " << hash_table.size 
              << " (" << (double)(hash_table.size - empty_buckets) / hash_table.size * 100 << "%)" << std::endl;
    std::cout << "  Порожні кошики: " << empty_buckets << std::endl;
    std::cout << "  Максимальна довжина ланцюжка: " << max_chain_length << std::endl;
    std::cout << "  Кількість операцій розширення: " << hash_table.resizes << std::endl;
    std::cout << "  Кількість операцій пошуку: " << hash_table.lookups << std::endl;
    std::cout << "  Загальна кількість перевірок при пошуку: " << hash_table.probes << std::endl;
    std::cout << "  Середня кількість перевірок на пошук: " << avg_probes << std::endl;
    
    std::cout << "\nРозподіл довжин ланцюжків:" << std::endl;
    for (size_t i = 0; i <= std::min(10, max_chain_length); i++) {
        std::cout << "  Довжина " << i << ": " << distribution[i] << " кошиків" << std::endl;
    }
    if (max_chain_length > 10) {
        std::cout << "  ..." << std::endl;
        std::cout << "  Довжина " << max_chain_length << ": " << distribution[max_chain_length] << " кошиків" << std::endl;
    }
}

// Демонстрація динамічного розширення хеш-таблиці
void demonstrate_dynamic_resizing() {
    std::cout << "Демонстрація динамічного розширення хеш-таблиці\n" << std::endl;
    
    // Ініціалізуємо хеш-таблицю
    init_hash_table(INITIAL_SIZE);
    
    std::cout << "Початковий стан хеш-таблиці:" << std::endl;
    analyze_hash_table();
    
    // Додаємо елементи, поки не відбудеться розширення
    std::cout << "\nДодаємо елементи, поки не відбудеться розширення..." << std::endl;
    
    int items_to_add = INITIAL_SIZE * LOAD_THRESHOLD * 2; // Достатньо для першого розширення
    for (int i = 0; i < items_to_add; i++) {
        char buffer[20];
        sprintf(buffer, "key%d", i);
        install(buffer, i * 10);
    }
    
    std::cout << "\nСтан після додавання " << items_to_add << " елементів:" << std::endl;
    analyze_hash_table();
    
    // Додаємо ще елементів для другого розширення
    std::cout << "\nДодаємо ще елементів для другого розширення..." << std::endl;
    
    for (int i = items_to_add; i < items_to_add * 2; i++) {
        char buffer[20];
        sprintf(buffer, "key%d", i);
        install(buffer, i * 10);
    }
    
    std::cout << "\nСтан після додавання ще " << items_to_add << " елементів:" << std::endl;
    analyze_hash_table();
    
    // Виконуємо пошук елементів для аналізу ефективності
    std::cout << "\nВиконуємо пошук випадкових елементів..." << std::endl;
    
    int searches = 1000;
    for (int i = 0; i < searches; i++) {
        int key_index = rand() % (items_to_add * 2);
        char buffer[20];
        sprintf(buffer, "key%d", key_index);
        lookup(buffer);
    }
    
    std::cout << "\nСтан після виконання " << searches << " пошуків:" << std::endl;
    analyze_hash_table();
}

// Функція для порівняння продуктивності з різними значеннями LOAD_THRESHOLD та GROWTH_FACTOR
void benchmark_parameters() {
    // Для повноцінного бенчмарку потрібно було б змінювати глобальні константи,
    // що вимагало б додаткової логіки. Тут наведено загальну структуру.
    
    std::cout << "\nПорівняння різних параметрів розширення хеш-таблиці" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    std::cout << "У повноцінному бенчмарку ми б порівняли різні комбінації:" << std::endl;
    std::cout << "  - LOAD_THRESHOLD: 0.5, 0.7, 0.9" << std::endl;
    std::cout << "  - GROWTH_FACTOR: 1.5, 2, 3" << std::endl;
    
    std::cout << "\nНаші поточні параметри:" << std::endl;
    std::cout << "  - LOAD_THRESHOLD: " << LOAD_THRESHOLD << std::endl;
    std::cout << "  - GROWTH_FACTOR: " << GROWTH_FACTOR << std::endl;
    
    std::cout << "\nТеоретичні висновки:" << std::endl;
    std::cout << "  1. Нижчий LOAD_THRESHOLD зменшує колізії, але збільшує витрати пам'яті" << std::endl;
    std::cout << "  2. Вищий GROWTH_FACTOR зменшує частоту розширень, але може" << std::endl;
    std::cout << "     призвести до неефективного використання пам'яті" << std::endl;
    std::cout << "  3. Оптимальні значення зазвичай: LOAD_THRESHOLD ≈ 0.7, GROWTH_FACTOR ≈ 2" << std::endl;
}

int main() {
    // Демонстрація динамічного розширення хеш-таблиці
    demonstrate_dynamic_resizing();
    
    // Порівняння продуктивності з різними параметрами
    benchmark_parameters();
    
    // Додаткові пояснення
    std::cout << "\nПереваги автоматичного розширення хеш-таблиці:" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "1. Адаптивність до кількості даних:" << std::endl;
    std::cout << "   - Таблиця автоматично розширюється при потребі" << std::endl;
    std::cout << "   - Немає необхідності заздалегідь знати розмір даних" << std::endl;
    
    std::cout << "\n2. Оптимізація продуктивності:" << std::endl;
    std::cout << "   - Підтримка невеликої середньої довжини ланцюжка" << std::endl;
    std::cout << "   - Час пошуку залишається близьким до O(1)" << std::endl;
    
    std::cout << "\n3. Ефективне використання пам'яті:" << std::endl;
    std::cout << "   - Початковий розмір може бути малим" << std::endl;
    std::cout << "   - Розмір збільшується лише при необхідності" << std::endl;
    
    std::cout << "\nНедоліки автоматичного розширення:" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "1. Затримка на перебудову:" << std::endl;
    std::cout << "   - Операція розширення потребує часу O(n)" << std::endl;
    std::cout << "   - Може спричинити затримку конкретної операції" << std::endl;
    
    std::cout << "\n2. Додаткові накладні витрати:" << std::endl;
    std::cout << "   - Відстеження середньої довжини ланцюжка" << std::endl;
    std::cout << "   - Додаткові перевірки при кожному пошуку" << std::endl;
    
    std::cout << "\nОптимальні параметри:" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "1. LOAD_THRESHOLD ≈ 0.7:" << std::endl;
    std::cout << "   - Баланс між ефективністю пошуку та використанням пам'яті" << std::endl;
    
    std::cout << "\n2. GROWTH_FACTOR ≈ 2:" << std::endl;
    std::cout << "   - Достатньо для амортизації вартості розширення" << std::endl;
    std::cout << "   - Не призводить до надмірного використання пам'яті" << std::endl;
    
    std::cout << "\n3. Використання простих чисел для розміру таблиці:" << std::endl;
    std::cout << "   - Зменшує ймовірність колізій" << std::endl;
    std::cout << "   - Покращує розподіл хеш-значень" << std::endl;
    
    // Звільнення пам'яті
    free_hash_table();
    
    return 0;
}