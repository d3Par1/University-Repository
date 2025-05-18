#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <functional>

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

// Структура для ітератора по хеш-таблиці
typedef struct {
    int bucket;         // Поточний кошик
    Nameval *current;   // Поточний елемент
    bool started;       // Чи був ітератор ініціалізований
} HashIterator;

// Ініціалізація ітератора
void init_iterator(HashIterator *it) {
    it->bucket = 0;
    it->current = NULL;
    it->started = false;
}

// Отримання наступного елемента хеш-таблиці
Nameval* next_element(HashIterator *it) {
    // Якщо ітератор ще не почав роботу
    if (!it->started) {
        // Шукаємо перший непорожній кошик
        while (it->bucket < NHASH && symtab[it->bucket] == NULL) {
            it->bucket++;
        }
        
        // Якщо всі кошики порожні
        if (it->bucket >= NHASH) {
            return NULL;
        }
        
        // Ініціалізуємо current першим елементом першого непорожнього кошика
        it->current = symtab[it->bucket];
        it->started = true;
        return it->current;
    }
    
    // Якщо є наступний елемент у поточному ланцюжку
    if (it->current->next != NULL) {
        it->current = it->current->next;
        return it->current;
    }
    
    // Якщо поточний ланцюжок закінчився, переходимо до наступного непорожнього кошика
    it->bucket++;
    while (it->bucket < NHASH && symtab[it->bucket] == NULL) {
        it->bucket++;
    }
    
    // Якщо всі кошики оброблені
    if (it->bucket >= NHASH) {
        return NULL;
    }
    
    // Ініціалізуємо current першим елементом нового кошика
    it->current = symtab[it->bucket];
    return it->current;
}

// Функція для обходу всіх елементів хеш-таблиці з використанням ітератора
void traverse_hash_table(void (*func)(Nameval*)) {
    HashIterator it;
    init_iterator(&it);
    
    Nameval *elem;
    while ((elem = next_element(&it)) != NULL) {
        func(elem);
    }
}

// Функція для обходу всіх елементів хеш-таблиці з використанням функції зворотного виклику та додаткового аргументу
void traverse_hash_table_with_arg(void (*func)(Nameval*, void*), void *arg) {
    HashIterator it;
    init_iterator(&it);
    
    Nameval *elem;
    while ((elem = next_element(&it)) != NULL) {
        func(elem, arg);
    }
}

// Альтернативний підхід - функція для обходу всіх елементів хеш-таблиці з використанням callback-функції
void for_each_in_hash_table(void (*callback)(Nameval *elem)) {
    for (int i = 0; i < NHASH; i++) {
        Nameval *sym;
        for (sym = symtab[i]; sym != NULL; sym = sym->next) {
            callback(sym);
        }
    }
}

// Обхід з використанням С++ функціональних об'єктів (std::function)
void for_each_in_hash_table_cpp(const std::function<void(Nameval*)>& callback) {
    for (int i = 0; i < NHASH; i++) {
        Nameval *sym;
        for (sym = symtab[i]; sym != NULL; sym = sym->next) {
            callback(sym);
        }
    }
}

// Функція для виведення елемента
void print_element(Nameval *elem) {
    std::cout << "  " << elem->name << " = " << elem->value << std::endl;
}

// Функція для виведення елемента з додатковим аргументом (лічильник)
void print_element_with_counter(Nameval *elem, void *arg) {
    int *counter = (int*)arg;
    std::cout << "  " << (*counter)++ << ". " << elem->name << " = " << elem->value << std::endl;
}

// Функція для підрахунку елементів
void count_element(Nameval *elem, void *arg) {
    int *counter = (int*)arg;
    (*counter)++;
}

// Функція для збору елементів у вектор
void collect_elements(std::vector<Nameval*>& elements) {
    for_each_in_hash_table_cpp([&elements](Nameval* elem) {
        elements.push_back(elem);
    });
}

// Функція для пошуку елементів, що відповідають певному критерію
void find_elements_greater_than(int threshold, std::vector<Nameval*>& results) {
    for_each_in_hash_table_cpp([&results, threshold](Nameval* elem) {
        if (elem->value > threshold) {
            results.push_back(elem);
        }
    });
}

// Демонстрація різних способів обходу хеш-таблиці
void demonstrate_hash_table_traversal() {
    std::cout << "Демонстрація обходу елементів хеш-таблиці:\n" << std::endl;
    
    // 1. Використання ітератора
    std::cout << "1. Обхід з використанням ітератора:" << std::endl;
    traverse_hash_table(print_element);
    
    // 2. Використання ітератора з додатковим аргументом
    std::cout << "\n2. Обхід з використанням ітератора та лічильника:" << std::endl;
    int counter = 1;
    traverse_hash_table_with_arg(print_element_with_counter, &counter);
    
    // 3. Підрахунок елементів
    std::cout << "\n3. Підрахунок елементів у хеш-таблиці:" << std::endl;
    int count = 0;
    traverse_hash_table_with_arg(count_element, &count);
    std::cout << "  Загальна кількість елементів: " << count << std::endl;
    
    // 4. Використання альтернативного підходу
    std::cout << "\n4. Обхід з використанням альтернативного підходу:" << std::endl;
    for_each_in_hash_table(print_element);
    
    // 5. Використання С++ функціональних об'єктів
    std::cout << "\n5. Обхід з використанням С++ функціональних об'єктів:" << std::endl;
    for_each_in_hash_table_cpp([](Nameval* elem) {
        std::cout << "  " << elem->name << " = " << elem->value << std::endl;
    });
    
    // 6. Збір елементів у вектор
    std::cout << "\n6. Збір елементів у вектор:" << std::endl;
    std::vector<Nameval*> elements;
    collect_elements(elements);
    std::cout << "  Зібрано " << elements.size() << " елементів" << std::endl;
    
    // 7. Пошук елементів, що відповідають критерію
    std::cout << "\n7. Пошук елементів зі значенням більше 50:" << std::endl;
    std::vector<Nameval*> results;
    find_elements_greater_than(50, results);
    std::cout << "  Знайдено " << results.size() << " елементів:" << std::endl;
    for (Nameval* elem : results) {
        std::cout << "    " << elem->name << " = " << elem->value << std::endl;
    }
}

// Перевірка продуктивності різних методів обходу
void benchmark_traversal_methods() {
    const int ITERATIONS = 100000;
    
    std::cout << "\nПорівняння продуктивності різних методів обходу:" << std::endl;
    
    // 1. Використання ітератора
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; i++) {
        int count = 0;
        traverse_hash_table_with_arg(count_element, &count);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // 2. Використання альтернативного підходу
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERATIONS; i++) {
        int count = 0;
        auto counter_func = [&count](Nameval* elem) { count++; };
        for_each_in_hash_table_cpp(counter_func);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Виведення результатів
    std::cout << "  Метод з ітератором: " << duration1 << " мс" << std::endl;
    std::cout << "  Метод з прямим обходом: " << duration2 << " мс" << std::endl;
    
    if (duration1 < duration2) {
        std::cout << "  Метод з ітератором швидший на " 
                  << std::fixed << std::setprecision(2) 
                  << (double)(duration2 - duration1) / duration2 * 100 << "%" << std::endl;
    } else {
        std::cout << "  Метод з прямим обходом швидший на " 
                  << std::fixed << std::setprecision(2) 
                  << (double)(duration1 - duration2) / duration1 * 100 << "%" << std::endl;
    }
}

int main() {
    // Ініціалізація хеш-таблиці
    for (int i = 0; i < NHASH; i++) {
        symtab[i] = NULL;
    }
    
    // Додавання тестових даних
    install((char*)"one", 1);
    install((char*)"two", 2);
    install((char*)"three", 3);
    install((char*)"four", 4);
    install((char*)"five", 5);
    install((char*)"ten", 10);
    install((char*)"twenty", 20);
    install((char*)"thirty", 30);
    install((char*)"forty", 40);
    install((char*)"fifty", 50);
    install((char*)"sixty", 60);
    install((char*)"seventy", 70);
    install((char*)"eighty", 80);
    install((char*)"ninety", 90);
    install((char*)"hundred", 100);
    
    // Демонстрація різних способів обходу хеш-таблиці
    demonstrate_hash_table_traversal();
    
    // Порівняння продуктивності різних методів обходу
    // benchmark_traversal_methods();  // Закоментовано для економії часу виконання
    
    // Пояснення та порівняння методів
    std::cout << "\nПорівняння методів обходу хеш-таблиці:" << std::endl;
    std::cout << "======================================" << std::endl;
    
    std::cout << "1. Метод з ітератором (traverse_hash_table):" << std::endl;
    std::cout << "   + Інкапсулює логіку обходу в окремому об'єкті (ітераторі)" << std::endl;
    std::cout << "   + Дозволяє зупинити і продовжити обхід у будь-який момент" << std::endl;
    std::cout << "   + Зручний для зовнішнього використання" << std::endl;
    std::cout << "   - Має додаткові накладні витрати на створення та управління ітератором" << std::endl;
    std::cout << "   - Складніша реалізація" << std::endl;
    
    std::cout << "\n2. Метод з прямим обходом (for_each_in_hash_table):" << std::endl;
    std::cout << "   + Простіша реалізація" << std::endl;
    std::cout << "   + Дещо ефективніший (менші накладні витрати)" << std::endl;
    std::cout << "   - Неможливо зупинити обхід посередині" << std::endl;
    std::cout << "   - Менш гнучкий" << std::endl;
    
    std::cout << "\n3. Метод з С++ функціональними об'єктами (for_each_in_hash_table_cpp):" << std::endl;
    std::cout << "   + Сучасний С++ підхід з використанням std::function" << std::endl;
    std::cout << "   + Підтримує лямбда-функції та функтори" << std::endl;
    std::cout << "   + Зручно використовувати з контейнерами С++" << std::endl;
    std::cout << "   - Доступний лише в С++, не в чистому C" << std::endl;
    std::cout << "   - Може мати додаткові накладні витрати для складних функціональних об'єктів" << std::endl;
    
    std::cout << "\nЗагальні рекомендації:" << std::endl;
    std::cout << "  - Для простих задач без необхідності зупинки обходу: for_each_in_hash_table" << std::endl;
    std::cout << "  - Для складних задач з можливістю зупинки: traverse_hash_table з ітератором" << std::endl;
    std::cout << "  - Для сучасного С++ коду: for_each_in_hash_table_cpp з лямбда-функціями" << std::endl;
    
    // Звільнення пам'яті
    free_symtab();
    
    return 0;
}