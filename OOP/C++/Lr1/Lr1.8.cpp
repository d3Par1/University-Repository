#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
ЗАВДАННЯ 8: Функція coolfun для гнучкого перебору списку

Реалізуємо універсальну функцію, яка перебирає елементи списку
та викликає задану функцію для кожного елемента окремо.
*/

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;
};

// Глобальний вказівник на початок списку
Nameval *nvlist = NULL;

// Створення нового елемента
Nameval* newitem(char *name, int value) {
    Nameval *newp = (Nameval *) malloc(sizeof(Nameval));
    if (newp == NULL) return NULL;

    newp->name = (char *) malloc(strlen(name) + 1);
    if (newp->name == NULL) {
        free(newp);
        return NULL;
    }

    strcpy(newp->name, name);
    newp->value = value;
    newp->next = NULL;
    return newp;
}

// Додавання елемента на початок списку
Nameval* addfront(Nameval *listp, Nameval *newp) {
    newp->next = listp;
    return newp;
}

// ОСНОВНА ФУНКЦІЯ: coolfun
// Перебирає список та викликає функцію fn для кожного елемента
void coolfun(Nameval *listp, void (*fn)(Nameval *, void *), void *arg) {
    for (Nameval *p = listp; p != NULL; p = p->next) {
        fn(p, arg);
    }
}

// ============= ПРИКЛАДИ ФУНКЦІЙ ДЛЯ ВИКОРИСТАННЯ З coolfun =============

// 1. Функція для підрахунку елементів (з прикладу в завданні)
void inccounter(Nameval *p, void *arg) {
    int *ip = (int *) arg;
    (*ip)++;
}

// 2. Функція для виведення елемента
void print_element(Nameval *p, void *arg) {
    char *prefix = (char *) arg;
    if (prefix != NULL) {
        printf("%s", prefix);
    }
    printf("%s = %d\n", p->name, p->value);
}

// 3. Функція для пошуку максимального значення
void find_max(Nameval *p, void *arg) {
    int *max_val = (int *) arg;
    if (p->value > *max_val) {
        *max_val = p->value;
    }
}

// 4. Функція для підрахунку суми всіх значень
void sum_values(Nameval *p, void *arg) {
    int *total = (int *) arg;
    *total += p->value;
}

// 5. Функція для пошуку елемента за ім'ям
struct search_result {
    char *target_name;
    Nameval *found;
};

void search_by_name(Nameval *p, void *arg) {
    struct search_result *search = (struct search_result *) arg;
    if (search->found == NULL && strcmp(p->name, search->target_name) == 0) {
        search->found = p;
    }
}

// 6. Функція для збору статистики
struct statistics {
    int count;
    int sum;
    int min;
    int max;
    double average;
};

void collect_stats(Nameval *p, void *arg) {
    struct statistics *stats = (struct statistics *) arg;

    if (stats->count == 0) {
        // Перший елемент
        stats->min = stats->max = p->value;
    } else {
        if (p->value < stats->min) stats->min = p->value;
        if (p->value > stats->max) stats->max = p->value;
    }

    stats->sum += p->value;
    stats->count++;
    stats->average = (double)stats->sum / stats->count;
}

// 7. Функція для фільтрації (збір елементів за умовою)
struct filter_data {
    int threshold;
    int *results;
    int result_count;
    int max_results;
};

void filter_greater_than(Nameval *p, void *arg) {
    struct filter_data *filter = (struct filter_data *) arg;

    if (p->value > filter->threshold && filter->result_count < filter->max_results) {
        filter->results[filter->result_count++] = p->value;
    }
}

// 8. Функція для валідації даних
struct validation_result {
    int valid_count;
    int invalid_count;
    char error_messages[1000];
};

void validate_element(Nameval *p, void *arg) {
    struct validation_result *validation = (struct validation_result *) arg;

    // Перевіряємо валідність даних
    int is_valid = 1;

    if (p->name == NULL || strlen(p->name) == 0) {
        strcat(validation->error_messages, "Порожнє ім'я; ");
        is_valid = 0;
    }

    if (p->value < 0) {
        strcat(validation->error_messages, "Від'ємне значення; ");
        is_valid = 0;
    }

    if (is_valid) {
        validation->valid_count++;
    } else {
        validation->invalid_count++;
    }
}

// Функція для звільнення пам'яті
void free_list(Nameval *listp) {
    Nameval *next;
    for (Nameval *p = listp; p != NULL; p = next) {
        next = p->next;
        free(p->name);
        free(p);
    }
}

void demonstrate_basic_usage() {
    printf("=== ОСНОВНЕ ВИКОРИСТАННЯ COOLFUN ===\n");

    // Приклад з завдання: підрахунок елементів
    int n = 0;
    coolfun(nvlist, inccounter, &n);
    printf("Кількість елементів у списку: %d\n", n);

    // Виведення всіх елементів
    printf("\nВсі елементи списку:\n");
    coolfun(nvlist, print_element, "  ");
}

void demonstrate_advanced_usage() {
    printf("\n=== РОЗШИРЕНЕ ВИКОРИСТАННЯ ===\n");

    // Пошук максимального значення
    int max_val = -1;
    coolfun(nvlist, find_max, &max_val);
    printf("Максимальне значення: %d\n", max_val);

    // Підрахунок суми
    int total = 0;
    coolfun(nvlist, sum_values, &total);
    printf("Сума всіх значень: %d\n", total);

    // Пошук за ім'ям
    struct search_result search = {"Charlie", NULL};
    coolfun(nvlist, search_by_name, &search);
    if (search.found != NULL) {
        printf("Знайдено елемент: %s = %d\n",
               search.found->name, search.found->value);
    } else {
        printf("Елемент 'Charlie' не знайдено\n");
    }

    // Збір детальної статистики
    struct statistics stats = {0, 0, 0, 0, 0.0};
    coolfun(nvlist, collect_stats, &stats);
    printf("\nСтатистика:\n");
    printf("  Кількість: %d\n", stats.count);
    printf("  Сума: %d\n", stats.sum);
    printf("  Мінімум: %d\n", stats.min);
    printf("  Максимум: %d\n", stats.max);
    printf("  Середнє: %.2f\n", stats.average);
}

void demonstrate_filtering() {
    printf("\n=== ДЕМОНСТРАЦІЯ ФІЛЬТРАЦІЇ ===\n");

    // Фільтруємо елементи зі значенням більше 25
    int results[10];
    struct filter_data filter = {25, results, 0, 10};

    coolfun(nvlist, filter_greater_than, &filter);

    printf("Елементи зі значенням більше %d:\n", filter.threshold);
    for (int i = 0; i < filter.result_count; i++) {
        printf("  Значення: %d\n", filter.results[i]);
    }
    printf("Знайдено: %d елементів\n", filter.result_count);
}

void demonstrate_validation() {
    printf("\n=== ДЕМОНСТРАЦІЯ ВАЛІДАЦІЇ ===\n");

    struct validation_result validation = {0, 0, ""};
    coolfun(nvlist, validate_element, &validation);

    printf("Результати валідації:\n");
    printf("  Валідних елементів: %d\n", validation.valid_count);
    printf("  Невалідних елементів: %d\n", validation.invalid_count);
    if (strlen(validation.error_messages) > 0) {
        printf("  Помилки: %s\n", validation.error_messages);
    }
}

void demonstrate_multiple_operations() {
    printf("\n=== КОМБІНУВАННЯ ОПЕРАЦІЙ ===\n");

    printf("Виконуємо кілька операцій послідовно:\n");

    // 1. Підрахунок
    int count = 0;
    coolfun(nvlist, inccounter, &count);
    printf("1. Підраховано елементів: %d\n", count);

    // 2. Сума
    int sum = 0;
    coolfun(nvlist, sum_values, &sum);
    printf("2. Обчислено суму: %d\n", sum);

    // 3. Пошук
    struct search_result search = {"Bob", NULL};
    coolfun(nvlist, search_by_name, &search);
    printf("3. Пошук 'Bob': %s\n",
           search.found ? "знайдено" : "не знайдено");

    // 4. Фільтрація
    int filter_results[10];
    struct filter_data filter = {30, filter_results, 0, 10};
    coolfun(nvlist, filter_greater_than, &filter);
    printf("4. Елементів > 30: %d\n", filter.result_count);

    printf("\nВсе виконано за 4 прохода по списку!\n");
}

int main(void) {
    printf("=== Завдання 8: Функція coolfun ===\n");

    // Створюємо тестовий список
    printf("\nСтворюємо тестовий список:\n");
    nvlist = addfront(nvlist, newitem("Eve", 50));
    nvlist = addfront(nvlist, newitem("David", 40));
    nvlist = addfront(nvlist, newitem("Charlie", 30));
    nvlist = addfront(nvlist, newitem("Bob", 20));
    nvlist = addfront(nvlist, newitem("Alice", 10));

    printf("Список створено: Alice(10) -> Bob(20) -> Charlie(30) -> David(40) -> Eve(50)\n");

    // Демонстрації
    demonstrate_basic_usage();
    demonstrate_advanced_usage();
    demonstrate_filtering();
    demonstrate_validation();
    demonstrate_multiple_operations();

    printf("\n=== ПЕРЕВАГИ COOLFUN ===\n");
    printf("1. ГНУЧКІСТЬ: одна функція для різних операцій\n");
    printf("2. ПОВТОРНЕ ВИКОРИСТАННЯ: можна комбінувати різні дії\n");
    printf("3. ЕФЕКТИВНІСТЬ: один прохід для однієї операції\n");
    printf("4. РОЗШИРЮВАНІСТЬ: легко додавати нові типи обробки\n");
    printf("5. ТИПОБЕЗПЕКА: використовує void* для універсальності\n\n");

    printf("=== ПРИКЛАДИ ВИКОРИСТАННЯ ===\n");
    printf("• Підрахунок елементів\n");
    printf("• Обчислення статистики (сума, мін, макс, середнє)\n");
    printf("• Пошук елементів за критерієм\n");
    printf("• Фільтрація та вибірка\n");
    printf("• Валідація даних\n");
    printf("• Трансформація даних\n");
    printf("• Агрегація результатів\n\n");

    printf("=== ПАТТЕРН ПРОЕКТУВАННЯ ===\n");
    printf("coolfun реалізує паттерн 'Visitor' - відділяє алгоритм\n");
    printf("обходу структури даних від операцій над елементами.\n");
    printf("Це дозволяє додавати нові операції без зміни структури списку.\n");

    // Звільняємо пам'ять
    free_list(nvlist);

    return 0;
}