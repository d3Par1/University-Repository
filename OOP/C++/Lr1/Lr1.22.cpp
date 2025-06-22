#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define INITIAL_NHASH 7          // Початковий розмір
#define LOAD_FACTOR_THRESHOLD 2.0 // Поріг для розширення
#define GROWTH_FACTOR 2          // Коефіцієнт зростання
#define MULTIPLIER 31

// ============= СТРУКТУРА ДИНАМІЧНОЇ ХЕШ-ТАБЛИЦІ =============

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;
};

typedef struct {
    Nameval **table;        // Масив вказівників на ланцюжки
    int size;              // Поточний розмір таблиці
    int count;             // Кількість елементів
    double load_factor;    // Поточний коефіцієнт завантаження
    int resizes;           // Лічильник розширень
    int total_operations;  // Загальна кількість операцій
} DynamicHashTable;

// Глобальна динамічна хеш-таблиця
static DynamicHashTable *dht = NULL;


unsigned int hash(char *str, int table_size) {
    unsigned int h = 0;
    unsigned char *p;

    for (p = (unsigned char *) str; *p != '\0'; p++) {
        h = MULTIPLIER * h + *p;
    }

    return h % table_size;
}


// Знаходження наступного простого числа
int next_prime(int n) {
    if (n <= 1) return 2;
    if (n <= 2) return 3;
    if (n % 2 == 0) n++;

    while (1) {
        int is_prime = 1;
        for (int i = 3; i * i <= n; i += 2) {
            if (n % i == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) return n;
        n += 2;
    }
}

// Обчислення середньої довжини ланцюжка
double calculate_average_chain_length(DynamicHashTable *table) {
    if (table->count == 0) return 0.0;

    int used_buckets = 0;
    for (int i = 0; i < table->size; i++) {
        if (table->table[i] != NULL) {
            used_buckets++;
        }
    }

    return used_buckets > 0 ? (double)table->count / used_buckets : 0.0;
}

// Обчислення максимальної довжини ланцюжка
int calculate_max_chain_length(DynamicHashTable *table) {
    int max_length = 0;

    for (int i = 0; i < table->size; i++) {
        int length = 0;
        for (Nameval *sym = table->table[i]; sym != NULL; sym = sym->next) {
            length++;
        }
        if (length > max_length) {
            max_length = length;
        }
    }

    return max_length;
}


DynamicHashTable* create_hash_table(int initial_size) {
    DynamicHashTable *table = (DynamicHashTable *) malloc(sizeof(DynamicHashTable));
    if (table == NULL) return NULL;

    table->size = next_prime(initial_size);
    table->count = 0;
    table->load_factor = 0.0;
    table->resizes = 0;
    table->total_operations = 0;

    table->table = (Nameval **) calloc(table->size, sizeof(Nameval *));
    if (table->table == NULL) {
        free(table);
        return NULL;
    }

    printf("Створено хеш-таблицю розміром %d\n", table->size);
    return table;
}

void destroy_hash_table(DynamicHashTable *table) {
    if (table == NULL) return;

    for (int i = 0; i < table->size; i++) {
        Nameval *sym = table->table[i];
        while (sym != NULL) {
            Nameval *next = sym->next;
            free(sym->name);
            free(sym);
            sym = next;
        }
    }

    free(table->table);
    free(table);
}


int resize_hash_table(DynamicHashTable *table, int new_size) {
    printf("\n=== РОЗШИРЕННЯ ТАБЛИЦІ ===\n");
    printf("Старий розмір: %d, новий розмір: %d\n", table->size, new_size);
    printf("Елементів для перехешування: %d\n", table->count);

    // Зберігаємо старі дані
    Nameval **old_table = table->table;
    int old_size = table->size;

    // Створюємо нову таблицю
    table->size = next_prime(new_size);
    table->table = (Nameval **) calloc(table->size, sizeof(Nameval *));
    if (table->table == NULL) {
        table->table = old_table;
        table->size = old_size;
        return 0; // Помилка
    }

    // Перехешування всіх елементів
    int rehashed = 0;
    for (int i = 0; i < old_size; i++) {
        Nameval *sym = old_table[i];
        while (sym != NULL) {
            Nameval *next = sym->next;

            // Обчислюємо новий hash
            unsigned int new_hash = hash(sym->name, table->size);

            // Вставляємо в нову таблицю
            sym->next = table->table[new_hash];
            table->table[new_hash] = sym;

            rehashed++;
            sym = next;
        }
    }

    // Звільняємо стару таблицю
    free(old_table);

    table->resizes++;
    table->load_factor = (double)table->count / table->size;

    printf("Перехешовано %d елементів\n", rehashed);
    printf("Новий коефіцієнт завантаження: %.3f\n", table->load_factor);
    printf("Кількість розширень: %d\n", table->resizes);

    return 1; // Успіх
}


// Модифікований lookup з автоматичним розширенням
Nameval* lookup_with_resize(char *name) {
    if (dht == NULL) return NULL;

    dht->total_operations++;

    // Перевіряємо необхідність розширення ПЕРЕД пошуком
    double avg_chain_length = calculate_average_chain_length(dht);

    if (avg_chain_length > LOAD_FACTOR_THRESHOLD) {
        printf("\nСередня довжина ланцюжка %.2f перевищує поріг %.2f\n",
               avg_chain_length, LOAD_FACTOR_THRESHOLD);

        int new_size = dht->size * GROWTH_FACTOR;
        if (resize_hash_table(dht, new_size)) {
            printf("Таблиця успішно розширена\n");
        } else {
            printf("ПОМИЛКА: Не вдалося розширити таблицю\n");
        }
    }

    // Виконуємо звичайний пошук
    unsigned int h = hash(name, dht->size);
    for (Nameval *sym = dht->table[h]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            return sym;
        }
    }

    return NULL;
}

// Звичайний lookup без розширення
Nameval* lookup_simple(char *name) {
    if (dht == NULL) return NULL;

    unsigned int h = hash(name, dht->size);
    for (Nameval *sym = dht->table[h]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            return sym;
        }
    }

    return NULL;
}

// Вставка елемента
Nameval* install(char *name, int value) {
    if (dht == NULL) {
        dht = create_hash_table(INITIAL_NHASH);
        if (dht == NULL) return NULL;
    }

    Nameval *sym = lookup_simple(name);

    if (sym == NULL) {
        // Новий елемент
        sym = (Nameval *) malloc(sizeof(Nameval));
        if (sym == NULL) return NULL;

        sym->name = (char *) malloc(strlen(name) + 1);
        if (sym->name == NULL) {
            free(sym);
            return NULL;
        }
        strcpy(sym->name, name);

        // Додаємо в таблицю
        unsigned int h = hash(name, dht->size);
        sym->next = dht->table[h];
        dht->table[h] = sym;

        dht->count++;
        dht->load_factor = (double)dht->count / dht->size;
    }

    sym->value = value;
    return sym;
}

// Видалення елемента
int uninstall(char *name) {
    if (dht == NULL) return 0;

    unsigned int h = hash(name, dht->size);
    Nameval *sym, *prev = NULL;

    for (sym = dht->table[h]; sym != NULL; prev = sym, sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            if (prev == NULL) {
                dht->table[h] = sym->next;
            } else {
                prev->next = sym->next;
            }

            free(sym->name);
            free(sym);
            dht->count--;
            dht->load_factor = (double)dht->count / dht->size;
            return 1;
        }
    }

    return 0;
}


typedef struct {
    int table_size;
    int element_count;
    double load_factor;
    double avg_chain_length;
    int max_chain_length;
    int empty_buckets;
    int resize_count;
    int total_operations;
} TableStats;

TableStats get_table_stats() {
    TableStats stats = {0};

    if (dht == NULL) return stats;

    stats.table_size = dht->size;
    stats.element_count = dht->count;
    stats.load_factor = dht->load_factor;
    stats.avg_chain_length = calculate_average_chain_length(dht);
    stats.max_chain_length = calculate_max_chain_length(dht);
    stats.resize_count = dht->resizes;
    stats.total_operations = dht->total_operations;

    // Підрахунок порожніх buckets
    for (int i = 0; i < dht->size; i++) {
        if (dht->table[i] == NULL) {
            stats.empty_buckets++;
        }
    }

    return stats;
}

void print_table_stats(const char *title) {
    printf("\n=== %s ===\n", title);

    if (dht == NULL) {
        printf("Таблиця не ініціалізована\n");
        return;
    }

    TableStats stats = get_table_stats();

    printf("Розмір таблиці: %d\n", stats.table_size);
    printf("Кількість елементів: %d\n", stats.element_count);
    printf("Коефіцієнт завантаження: %.3f\n", stats.load_factor);
    printf("Середня довжина ланцюжка: %.2f\n", stats.avg_chain_length);
    printf("Максимальна довжина ланцюжка: %d\n", stats.max_chain_length);
    printf("Порожніх buckets: %d (%.1f%%)\n",
           stats.empty_buckets,
           (double)stats.empty_buckets / stats.table_size * 100);
    printf("Кількість розширень: %d\n", stats.resize_count);
    printf("Загальна кількість операцій: %d\n", stats.total_operations);

    // Оцінка ефективності
    if (stats.avg_chain_length <= 1.5) {
        printf("Ефективність: ВІДМІННА ✓\n");
    } else if (stats.avg_chain_length <= 2.5) {
        printf("Ефективність: ДОБРА\n");
    } else if (stats.avg_chain_length <= 4.0) {
        printf("Ефективність: ЗАДОВІЛЬНА\n");
    } else {
        printf("Ефективність: ПОГАНА \n");
    }
}

void print_bucket_distribution() {
    if (dht == NULL) return;

    printf("\nРозподіл довжин ланцюжків:\n");

    int max_length = calculate_max_chain_length(dht);
    int *distribution = (int *) calloc(max_length + 1, sizeof(int));

    for (int i = 0; i < dht->size; i++) {
        int length = 0;
        for (Nameval *sym = dht->table[i]; sym != NULL; sym = sym->next) {
            length++;
        }
        distribution[length]++;
    }

    for (int i = 0; i <= max_length; i++) {
        if (distribution[i] > 0) {
            printf("  Довжина %d: %d buckets", i, distribution[i]);

            // Проста гістограма
            int bar_length = distribution[i] * 30 / dht->size;
            printf(" ");
            for (int j = 0; j < bar_length; j++) {
                printf("█");
            }
            printf("\n");
        }
    }

    free(distribution);
}


void demonstrate_basic_resizing() {
    printf("=== ДЕМОНСТРАЦІЯ БАЗОВОГО РОЗШИРЕННЯ ===\n");

    if (dht != NULL) {
        destroy_hash_table(dht);
    }
    dht = create_hash_table(INITIAL_NHASH);

    print_table_stats("Початковий стан");

    // Додаємо елементи поступово
    char *items[] = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon",
        "mango", "nectarine", "orange", "peach", "quince",
        "raspberry", "strawberry", "tangerine", "watermelon"
    };

    int item_count = sizeof(items) / sizeof(items[0]);

    for (int i = 0; i < item_count; i++) {
        printf("\n--- Додаємо '%s' (елемент %d) ---\n", items[i], i + 1);

        install(items[i], i * 10);

        // Перевіряємо стан після кожного додавання
        TableStats stats = get_table_stats();
        printf("Стан: %d елементів, середня довжина ланцюжка: %.2f\n",
               stats.element_count, stats.avg_chain_length);

        // Тестуємо lookup з можливим розширенням
        Nameval *found = lookup_with_resize(items[i]);
        if (found) {
            printf("Знайдено: %s = %d\n", found->name, found->value);
        }

        // Показуємо статистику після потенційного розширення
        if (i % 5 == 4) { // Кожні 5 елементів
            print_table_stats("Поточна статистика");
        }
    }

    print_table_stats("Фінальний стан");
    print_bucket_distribution();
}

void demonstrate_threshold_testing() {
    printf("\n=== ТЕСТУВАННЯ РІЗНИХ ПОРОГІВ ===\n");

    double thresholds[] = {1.5, 2.0, 3.0, 4.0};
    int threshold_count = sizeof(thresholds) / sizeof(thresholds[0]);

    for (int t = 0; t < threshold_count; t++) {
        printf("\n--- Тест з порогом %.1f ---\n", thresholds[t]);

        if (dht != NULL) {
            destroy_hash_table(dht);
        }
        dht = create_hash_table(7);

        // Тимчасово змінюємо поріг (в реальності це була б змінна)
        printf("Поріг встановлено на %.1f\n", thresholds[t]);

        // Додаємо 20 елементів
        for (int i = 0; i < 20; i++) {
            char name[20];
            sprintf(name, "item_%d", i);
            install(name, i);

            // Перевіряємо необхідність розширення вручну
            double avg_length = calculate_average_chain_length(dht);
            if (avg_length > thresholds[t] && dht->size < 100) {
                printf("  Розширення на елементі %d (довжина: %.2f)\n",
                       i + 1, avg_length);
                resize_hash_table(dht, dht->size * 2);
            }
        }

        TableStats final_stats = get_table_stats();
        printf("Результат: розмір %d, розширень %d, середня довжина %.2f\n",
               final_stats.table_size, final_stats.resize_count,
               final_stats.avg_chain_length);
    }
}

void demonstrate_performance_comparison() {
    printf("\n=== ПОРІВНЯННЯ ПРОДУКТИВНОСТІ ===\n");

    const int TEST_SIZE = 1000;

    // Тест 1: З динамічним розширенням
    if (dht != NULL) {
        destroy_hash_table(dht);
    }
    dht = create_hash_table(7);

    clock_t start = clock();
    for (int i = 0; i < TEST_SIZE; i++) {
        char name[20];
        sprintf(name, "dynamic_%d", i);
        install(name, i);
        lookup_with_resize(name); // Використовуємо lookup з розширенням
    }
    clock_t end = clock();

    double dynamic_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    TableStats dynamic_stats = get_table_stats();

    // Тест 2: З фіксованим розміром (великим)
    destroy_hash_table(dht);
    dht = create_hash_table(1009); // Великий фіксований розмір

    start = clock();
    for (int i = 0; i < TEST_SIZE; i++) {
        char name[20];
        sprintf(name, "fixed_%d", i);
        install(name, i);
        lookup_simple(name); // Звичайний lookup
    }
    end = clock();

    double fixed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    TableStats fixed_stats = get_table_stats();

    // Порівняння результатів
    printf("Результати для %d операцій:\n\n", TEST_SIZE * 2);

    printf("ДИНАМІЧНА ТАБЛИЦЯ:\n");
    printf("  Час: %.6f сек\n", dynamic_time);
    printf("  Розмір: %d\n", dynamic_stats.table_size);
    printf("  Розширень: %d\n", dynamic_stats.resize_count);
    printf("  Середня довжина: %.2f\n", dynamic_stats.avg_chain_length);

    printf("\nФІКСОВАНА ТАБЛИЦЯ:\n");
    printf("  Час: %.6f сек\n", fixed_time);
    printf("  Розмір: %d\n", fixed_stats.table_size);
    printf("  Розширень: %d\n", fixed_stats.resize_count);
    printf("  Середня довжина: %.2f\n", fixed_stats.avg_chain_length);

    printf("\nВИСНОВОК:\n");
    if (dynamic_time < fixed_time * 1.5) {
        printf("Динамічна таблиця показала хорошу продуктивність\n");
    } else {
        printf("Фіксована таблиця швидша через відсутність розширень\n");
    }

    printf("Економія пам'яті: %.1f%% (динамічна vs фіксована)\n",
           (1.0 - (double)dynamic_stats.table_size / fixed_stats.table_size) * 100);
}

void demonstrate_edge_cases() {
    printf("\n=== ТЕСТУВАННЯ КРАЙНІХ ВИПАДКІВ ===\n");

    // Випадок 1: Багато колізій
    printf("\n--- Випадок 1: Примусові колізії ---\n");
    if (dht != NULL) {
        destroy_hash_table(dht);
    }
    dht = create_hash_table(7);

    // Створюємо рядки, які мають однаковий hash
    for (int i = 0; i < 15; i++) {
        char name[20];
        sprintf(name, "collision_%d", i * 7); // Намагаємося створити колізії
        install(name, i);
    }

    print_table_stats("Після примусових колізій");

    // Випадок 2: Швидке видалення після розширення
    printf("\n--- Випадок 2: Видалення після розширення ---\n");

    // Видаляємо половину елементів
    for (int i = 0; i < 7; i++) {
        char name[20];
        sprintf(name, "collision_%d", i * 7);
        uninstall(name);
    }

    print_table_stats("Після видалення половини елементів");

    // Випадок 3: Повторне заповнення
    printf("\n--- Випадок 3: Повторне заповнення ---\n");
    for (int i = 0; i < 20; i++) {
        char name[20];
        sprintf(name, "refill_%d", i);
        install(name, i);
    }

    print_table_stats("Після повторного заповнення");
}

int main(void) {

    demonstrate_basic_resizing();
    demonstrate_threshold_testing();
    demonstrate_performance_comparison();
    demonstrate_edge_cases();

    if (dht != NULL) {
        destroy_hash_table(dht);
        dht = NULL;
    }

    return 0;
}