#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
ЗАВДАННЯ 21: Функція для послідовного звернення до елементів хеш-таблиці

Реалізуємо функцію для послідовного обходу всіх елементів хеш-таблиці
в невідсортованому порядку, дозволяючи ітерацію по всій таблиці.
*/

// ============= ПАРАМЕТРИ ХЕШУВАННЯ =============

#define NHASH 101
#define MULTIPLIER 31

// ============= СТРУКТУРА ХЕШ-ТАБЛИЦІ =============

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;
};

// Глобальна хеш-таблиця
static Nameval *symtab[NHASH];

// ============= СТРУКТУРА ДЛЯ ІТЕРАТОРА =============

typedef struct {
    int current_bucket;      // Поточний bucket
    Nameval *current_node;   // Поточний елемент в ланцюжку
    int total_visited;       // Кількість відвіданих елементів
    int is_finished;         // Прапор завершення ітерації
} HashIterator;

// ============= ОСНОВНІ ФУНКЦІЇ ХЕШ-ТАБЛИЦІ =============

unsigned int hash(char *str) {
    unsigned int h = 0;
    unsigned char *p;

    for (p = (unsigned char *) str; *p != '\0'; p++) {
        h = MULTIPLIER * h + *p;
    }

    return h % NHASH;
}

Nameval* lookup(char *name) {
    Nameval *sym;

    for (sym = symtab[hash(name)]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) {
            return sym;
        }
    }
    return NULL;
}

Nameval* install(char *name, int value) {
    Nameval *sym;
    unsigned int h;

    if ((sym = lookup(name)) == NULL) {
        sym = (Nameval *) malloc(sizeof(Nameval));
        if (sym == NULL) return NULL;

        sym->name = (char *) malloc(strlen(name) + 1);
        if (sym->name == NULL) {
            free(sym);
            return NULL;
        }
        strcpy(sym->name, name);

        h = hash(name);
        sym->next = symtab[h];
        symtab[h] = sym;
    }
    sym->value = value;
    return sym;
}

// ============= ФУНКЦІЇ ПОСЛІДОВНОГО ОБХОДУ =============

// Ініціалізація ітератора
void hash_iterator_init(HashIterator *iter) {
    iter->current_bucket = 0;
    iter->current_node = NULL;
    iter->total_visited = 0;
    iter->is_finished = 0;

    // Знаходимо перший непорожній bucket
    while (iter->current_bucket < NHASH && symtab[iter->current_bucket] == NULL) {
        iter->current_bucket++;
    }

    if (iter->current_bucket < NHASH) {
        iter->current_node = symtab[iter->current_bucket];
    } else {
        iter->is_finished = 1;
    }
}

// Отримання наступного елемента
Nameval* hash_iterator_next(HashIterator *iter) {
    if (iter->is_finished) {
        return NULL;
    }

    Nameval *result = iter->current_node;
    iter->total_visited++;

    // Переходимо до наступного елемента
    if (iter->current_node->next != NULL) {
        // Наступний елемент в тому ж ланцюжку
        iter->current_node = iter->current_node->next;
    } else {
        // Шукаємо наступний непорожній bucket
        iter->current_bucket++;
        while (iter->current_bucket < NHASH && symtab[iter->current_bucket] == NULL) {
            iter->current_bucket++;
        }

        if (iter->current_bucket < NHASH) {
            iter->current_node = symtab[iter->current_bucket];
        } else {
            iter->is_finished = 1;
            iter->current_node = NULL;
        }
    }

    return result;
}

// Перевірка, чи є ще елементи
int hash_iterator_has_next(HashIterator *iter) {
    return !iter->is_finished;
}

// Отримання інформації про прогрес ітерації
void hash_iterator_status(HashIterator *iter) {
    printf("Ітератор: bucket %d/%d, відвідано %d елементів, %s\n",
           iter->current_bucket, NHASH - 1, iter->total_visited,
           iter->is_finished ? "завершено" : "активний");
}

// ============= АЛЬТЕРНАТИВНІ МЕТОДИ ОБХОДУ =============

// Метод 1: Простий послідовний обхід з callback функцією
void hash_table_foreach(void (*callback)(Nameval *item, void *data), void *user_data) {
    for (int i = 0; i < NHASH; i++) {
        for (Nameval *sym = symtab[i]; sym != NULL; sym = sym->next) {
            callback(sym, user_data);
        }
    }
}

// Метод 2: Збір всіх елементів у масив
Nameval** hash_table_to_array(int *count) {
    // Спочатку підраховуємо кількість елементів
    int total = 0;
    for (int i = 0; i < NHASH; i++) {
        for (Nameval *sym = symtab[i]; sym != NULL; sym = sym->next) {
            total++;
        }
    }

    if (total == 0) {
        *count = 0;
        return NULL;
    }

    // Виділяємо пам'ять для масиву
    Nameval **array = (Nameval **) malloc(total * sizeof(Nameval *));
    if (array == NULL) {
        *count = 0;
        return NULL;
    }

    // Заповнюємо масив
    int index = 0;
    for (int i = 0; i < NHASH; i++) {
        for (Nameval *sym = symtab[i]; sym != NULL; sym = sym->next) {
            array[index++] = sym;
        }
    }

    *count = total;
    return array;
}

// Метод 3: Обхід з можливістю фільтрації
void hash_table_foreach_filtered(int (*filter)(Nameval *item),
                                void (*callback)(Nameval *item, void *data),
                                void *user_data) {
    for (int i = 0; i < NHASH; i++) {
        for (Nameval *sym = symtab[i]; sym != NULL; sym = sym->next) {
            if (filter == NULL || filter(sym)) {
                callback(sym, user_data);
            }
        }
    }
}

// Метод 4: Обхід з інформацією про позицію
typedef struct {
    Nameval *item;
    int bucket;
    int position_in_bucket;
    int global_position;
} HashTableEntry;

void hash_table_foreach_with_position(void (*callback)(HashTableEntry *entry, void *data),
                                     void *user_data) {
    HashTableEntry entry;
    entry.global_position = 0;

    for (int i = 0; i < NHASH; i++) {
        entry.bucket = i;
        entry.position_in_bucket = 0;

        for (Nameval *sym = symtab[i]; sym != NULL; sym = sym->next) {
            entry.item = sym;
            callback(&entry, user_data);

            entry.position_in_bucket++;
            entry.global_position++;
        }
    }
}

// ============= CALLBACK ФУНКЦІЇ ДЛЯ ДЕМОНСТРАЦІЇ =============

// Callback для простого виведення
void print_item(Nameval *item, void *data) {
    printf("  %s = %d\n", item->name, item->value);
}

// Callback для підрахунку елементів
void count_items(Nameval *item, void *data) {
    int *counter = (int *) data;
    (*counter)++;
}

// Callback для пошуку максимального значення
void find_max_value(Nameval *item, void *data) {
    int *max_val = (int *) data;
    if (item->value > *max_val) {
        *max_val = item->value;
    }
}

// Callback для збору статистики
typedef struct {
    int total_items;
    int total_name_length;
    int min_value;
    int max_value;
    long sum_values;
} TableStats;

void collect_stats(Nameval *item, void *data) {
    TableStats *stats = (TableStats *) data;

    stats->total_items++;
    stats->total_name_length += strlen(item->name);
    stats->sum_values += item->value;

    if (stats->total_items == 1) {
        stats->min_value = stats->max_value = item->value;
    } else {
        if (item->value < stats->min_value) stats->min_value = item->value;
        if (item->value > stats->max_value) stats->max_value = item->value;
    }
}

// Callback з інформацією про позицію
void print_item_with_position(HashTableEntry *entry, void *data) {
    printf("  [%d:%d] (global %d) %s = %d\n",
           entry->bucket, entry->position_in_bucket, entry->global_position,
           entry->item->name, entry->item->value);
}

// Фільтр для елементів з великими значеннями
int filter_high_values(Nameval *item) {
    return item->value > 50;
}

// Фільтр для коротких імен
int filter_short_names(Nameval *item) {
    return strlen(item->name) <= 5;
}

// ============= ДОПОМІЖНІ ФУНКЦІЇ =============

void clear_hash_table() {
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

void populate_test_data() {
    char *test_data[][2] = {
        {"apple", "10"}, {"banana", "25"}, {"cherry", "15"},
        {"date", "8"}, {"elderberry", "45"}, {"fig", "12"},
        {"grape", "30"}, {"honeydew", "22"}, {"kiwi", "18"},
        {"lemon", "35"}, {"mango", "40"}, {"nectarine", "28"},
        {"orange", "33"}, {"peach", "20"}, {"quince", "55"},
        {"raspberry", "65"}, {"strawberry", "42"}, {"tangerine", "38"}
    };

    int count = sizeof(test_data) / sizeof(test_data[0]);

    for (int i = 0; i < count; i++) {
        install(test_data[i][0], atoi(test_data[i][1]));
    }
}

// ============= ДЕМОНСТРАЦІЇ =============

void demonstrate_iterator() {
    printf("=== ДЕМОНСТРАЦІЯ ІТЕРАТОРА ===\n");

    clear_hash_table();
    populate_test_data();

    printf("Обходимо хеш-таблицю за допомогою ітератора:\n");

    HashIterator iter;
    hash_iterator_init(&iter);

    int item_count = 0;
    while (hash_iterator_has_next(&iter)) {
        Nameval *item = hash_iterator_next(&iter);
        printf("  [%d] %s = %d\n", item_count + 1, item->name, item->value);
        item_count++;

        // Показуємо статус кожні 5 елементів
        if ((item_count % 5) == 0) {
            hash_iterator_status(&iter);
        }
    }

    printf("\nЗагалом оброблено %d елементів\n", item_count);
    hash_iterator_status(&iter);
}

void demonstrate_foreach_methods() {
    printf("\n=== ДЕМОНСТРАЦІЯ РІЗНИХ МЕТОДІВ ОБХОДУ ===\n");

    // Метод 1: Простий foreach
    printf("\n--- Метод 1: Простий foreach ---\n");
    hash_table_foreach(print_item, NULL);

    // Метод 2: Підрахунок елементів
    printf("\n--- Метод 2: Підрахунок елементів ---\n");
    int count = 0;
    hash_table_foreach(count_items, &count);
    printf("Загалом елементів: %d\n", count);

    // Метод 3: Пошук максимального значення
    printf("\n--- Метод 3: Пошук максимального значення ---\n");
    int max_value = -1;
    hash_table_foreach(find_max_value, &max_value);
    printf("Максимальне значення: %d\n", max_value);

    // Метод 4: Збір статистики
    printf("\n--- Метод 4: Збір статистики ---\n");
    TableStats stats = {0, 0, 0, 0, 0};
    hash_table_foreach(collect_stats, &stats);

    printf("Статистика таблиці:\n");
    printf("  Елементів: %d\n", stats.total_items);
    printf("  Середня довжина імені: %.1f\n",
           (double)stats.total_name_length / stats.total_items);
    printf("  Мін. значення: %d\n", stats.min_value);
    printf("  Макс. значення: %d\n", stats.max_value);
    printf("  Середнє значення: %.1f\n",
           (double)stats.sum_values / stats.total_items);
}

void demonstrate_array_conversion() {
    printf("\n=== ДЕМОНСТРАЦІЯ КОНВЕРТАЦІЇ У МАСИВ ===\n");

    int array_count;
    Nameval **array = hash_table_to_array(&array_count);

    if (array != NULL) {
        printf("Хеш-таблиця конвертована у масив з %d елементів:\n", array_count);

        for (int i = 0; i < array_count; i++) {
            printf("  array[%d] = %s (%d)\n", i, array[i]->name, array[i]->value);
        }

        // Тепер можна сортувати масив, якщо потрібно
        printf("\nПереваги масиву:\n");
        printf("• Можна сортувати\n");
        printf("• Швидкий доступ за індексом\n");
        printf("• Легко передавати іншим функціям\n");

        free(array);
    } else {
        printf("Таблиця порожня або помилка виділення пам'яті\n");
    }
}

void demonstrate_filtered_traversal() {
    printf("\n=== ДЕМОНСТРАЦІЯ ФІЛЬТРОВАНОГО ОБХОДУ ===\n");

    printf("\n--- Фільтр 1: Тільки великі значення (> 50) ---\n");
    hash_table_foreach_filtered(filter_high_values, print_item, NULL);

    printf("\n--- Фільтр 2: Тільки короткі імена (<= 5 символів) ---\n");
    hash_table_foreach_filtered(filter_short_names, print_item, NULL);

    printf("\n--- Без фільтра (всі елементи) ---\n");
    hash_table_foreach_filtered(NULL, print_item, NULL);
}

void demonstrate_positional_traversal() {
    printf("\n=== ДЕМОНСТРАЦІЯ ОБХОДУ З ПОЗИЦІЙНОЮ ІНФОРМАЦІЄЮ ===\n");

    printf("Елементи з інформацією про розташування:\n");
    hash_table_foreach_with_position(print_item_with_position, NULL);
}

void performance_comparison() {
    printf("\n=== ПОРІВНЯННЯ ПРОДУКТИВНОСТІ ===\n");

    // Додаємо більше даних для тестування
    for (int i = 0; i < 1000; i++) {
        char name[20];
        sprintf(name, "item_%d", i);
        install(name, i);
    }

    clock_t start, end;
    int count;

    // Тест 1: Ітератор
    start = clock();
    HashIterator iter;
    hash_iterator_init(&iter);
    count = 0;
    while (hash_iterator_has_next(&iter)) {
        hash_iterator_next(&iter);
        count++;
    }
    end = clock();
    double iterator_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Тест 2: Foreach
    start = clock();
    count = 0;
    hash_table_foreach(count_items, &count);
    end = clock();
    double foreach_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Тест 3: Конвертація у масив
    start = clock();
    Nameval **array = hash_table_to_array(&count);
    end = clock();
    double array_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Результати для %d елементів:\n", count);
    printf("  Ітератор:      %.6f сек\n", iterator_time);
    printf("  Foreach:       %.6f сек\n", foreach_time);
    printf("  Масив:         %.6f сек\n", array_time);

    if (array) free(array);
}

int main(void) {
    printf("=== Завдання 21: Послідовний обхід хеш-таблиці ===\n");

    demonstrate_iterator();
    demonstrate_foreach_methods();
    demonstrate_array_conversion();
    demonstrate_filtered_traversal();
    demonstrate_positional_traversal();
    performance_comparison();

    printf("\n=== ПІДСУМОК РЕАЛІЗОВАНИХ МЕТОДІВ ===\n");

    printf("\n1. ІТЕРАТОР (HashIterator):\n");
    printf("   + Контроль над процесом ітерації\n");
    printf("   + Можливість зупинки та продовження\n");
    printf("   + Мінімальне використання пам'яті\n");
    printf("   - Більш складний у використанні\n\n");

    printf("2. FOREACH З CALLBACK:\n");
    printf("   + Простота використання\n");
    printf("   + Гнучкість через callback функції\n");
    printf("   + Компактний код\n");
    printf("   - Складніше зупинити передчасно\n\n");

    printf("3. КОНВЕРТАЦІЯ У МАСИВ:\n");
    printf("   + Можливість сортування\n");
    printf("   + Швидкий випадковий доступ\n");
    printf("   + Сумісність з іншими алгоритмами\n");
    printf("   - Додаткове використання пам'яті\n\n");

    printf("4. ФІЛЬТРОВАНИЙ ОБХІД:\n");
    printf("   + Обробка тільки потрібних елементів\n");
    printf("   + Ефективність для вибіркових операцій\n");
    printf("   + Композиція фільтрів\n");
    printf("   - Потребує написання фільтр-функцій\n\n");

    printf("5. ПОЗИЦІЙНИЙ ОБХІД:\n");
    printf("   + Додаткова інформація про розташування\n");
    printf("   + Корисно для налагодження\n");
    printf("   + Аналіз розподілу елементів\n");
    printf("   - Невеликі накладні витрати\n\n");

    printf("=== РЕКОМЕНДАЦІЇ ПО ВИКОРИСТАННЮ ===\n");
    printf("• Простий перебір → foreach\n");
    printf("• Контрольований обхід → ітератор\n");
    printf("• Сортування результатів → масив\n");
    printf("• Вибіркова обробка → фільтрований обхід\n");
    printf("• Аналіз структури → позиційний обхід\n");

    clear_hash_table();
    return 0;
}