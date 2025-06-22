#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


static int NHASH = 101;          // Розмір хеш-таблиці
static int MULTIPLIER = 31;      // Змінний множник для тестування


typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;
};


// Основна хеш-функція (з можливістю зміни множника)
unsigned int hash_with_multiplier(char *str, int multiplier) {
    unsigned int h = 0;
    unsigned char *p;

    for (p = (unsigned char *) str; *p != '\0'; p++) {
        h = multiplier * h + *p;
    }

    return h % NHASH;
}

// Стандартна хеш-функція
unsigned int hash(char *str) {
    return hash_with_multiplier(str, MULTIPLIER);
}

// Альтернативна хеш-функція для порівняння (djb2)
unsigned int hash_djb2(char *str) {
    unsigned int hash = 5381;
    unsigned char *p;

    for (p = (unsigned char *) str; *p != '\0'; p++) {
        hash = ((hash << 5) + hash) + *p; // hash * 33 + c
    }

    return hash % NHASH;
}


typedef struct {
    int total_items;
    int used_buckets;
    int empty_buckets;
    int max_chain_length;
    int total_collisions;
    double load_factor;
    double average_chain_length;
    double uniformity_score;
    char description[100];
} HashQuality;

// Обчислення якості розподілу
HashQuality analyze_distribution(char **strings, int count,
                                unsigned int (*hash_func)(char *),
                                const char *func_name) {
    HashQuality quality = {0};
    strcpy(quality.description, func_name);

    // Лічильники для кожного bucket
    int bucket_counts[NHASH] = {0};

    // Розподіляємо рядки по buckets
    for (int i = 0; i < count; i++) {
        unsigned int h = hash_func(strings[i]);
        bucket_counts[h]++;
    }

    // Аналізуємо розподіл
    quality.total_items = count;

    for (int i = 0; i < NHASH; i++) {
        if (bucket_counts[i] > 0) {
            quality.used_buckets++;

            if (bucket_counts[i] > quality.max_chain_length) {
                quality.max_chain_length = bucket_counts[i];
            }

            if (bucket_counts[i] > 1) {
                quality.total_collisions += (bucket_counts[i] - 1);
            }
        } else {
            quality.empty_buckets++;
        }
    }

    quality.load_factor = (double)count / NHASH;
    quality.average_chain_length = quality.used_buckets > 0 ?
        (double)count / quality.used_buckets : 0.0;

    // Розрахунок однорідності (наскільки рівномірний розподіл)
    double expected_per_bucket = (double)count / NHASH;
    double variance = 0.0;

    for (int i = 0; i < NHASH; i++) {
        double diff = bucket_counts[i] - expected_per_bucket;
        variance += diff * diff;
    }
    variance /= NHASH;

    // Нормалізуємо: 1.0 = ідеальний розподіл, 0.0 = найгірший
    quality.uniformity_score = 1.0 / (1.0 + variance / expected_per_bucket);

    return quality;
}

void print_hash_quality(HashQuality quality) {
    printf("\n=== %s ===\n", quality.description);
    printf("Загалом елементів: %d\n", quality.total_items);
    printf("Використано buckets: %d/%d (%.1f%%)\n",
           quality.used_buckets, NHASH,
           (double)quality.used_buckets / NHASH * 100);
    printf("Коефіцієнт завантаження: %.3f\n", quality.load_factor);
    printf("Макс. довжина ланцюжка: %d\n", quality.max_chain_length);
    printf("Середня довжина ланцюжка: %.2f\n", quality.average_chain_length);
    printf("Колізій: %d\n", quality.total_collisions);
    printf("Однорідність розподілу: %.3f\n", quality.uniformity_score);

    // Оцінка якості
    if (quality.uniformity_score >= 0.8 && quality.max_chain_length <= 3) {
        printf("Оцінка: ВІДМІННО ✓\n");
    } else if (quality.uniformity_score >= 0.6 && quality.max_chain_length <= 5) {
        printf("Оцінка: ДОБРЕ\n");
    } else if (quality.uniformity_score >= 0.4 && quality.max_chain_length <= 8) {
        printf("Оцінка: ЗАДОВІЛЬНО\n");
    } else {
        printf("Оцінка: ПОГАНО ✗\n");
    }
}


// Короткі рядки (імена змінних)
void generate_short_strings(char **strings, int count) {
    char prefixes[][10] = {"i", "j", "k", "x", "y", "z", "tmp", "buf", "ptr", "val",
                          "len", "max", "min", "sum", "cnt", "idx", "pos", "size"};
    int prefix_count = sizeof(prefixes) / sizeof(prefixes[0]);

    for (int i = 0; i < count; i++) {
        strings[i] = (char *) malloc(20);
        if (i < prefix_count) {
            strcpy(strings[i], prefixes[i]);
        } else {
            sprintf(strings[i], "%s%d", prefixes[i % prefix_count], i / prefix_count);
        }
    }
}

// Довгі рядки (URL-адреси)
void generate_long_strings(char **strings, int count) {
    char domains[][25] = {"example.com", "test.org", "demo.net", "site.info",
                         "portal.edu", "service.gov", "shop.biz"};
    char sections[][15] = {"api", "admin", "user", "content", "media", "docs", "help"};
    char resources[][15] = {"page", "article", "post", "image", "video", "file", "data"};

    int domain_count = sizeof(domains) / sizeof(domains[0]);
    int section_count = sizeof(sections) / sizeof(sections[0]);
    int resource_count = sizeof(resources) / sizeof(resources[0]);

    for (int i = 0; i < count; i++) {
        strings[i] = (char *) malloc(200);
        sprintf(strings[i], "https://www.%s/%s/v%d/%s/%d/%s%d.html",
                domains[i % domain_count],
                sections[i % section_count],
                (i / 10) % 5 + 1,
                resources[i % resource_count],
                i % 1000,
                resources[(i + 1) % resource_count],
                i % 100);
    }
}

// Рядки однакової довжини з невеликими відмінностями
void generate_similar_strings(char **strings, int count, int length) {
    for (int i = 0; i < count; i++) {
        strings[i] = (char *) malloc(length + 1);

        // Базовий паттерн
        for (int j = 0; j < length; j++) {
            strings[i][j] = 'a' + (j % 26);
        }
        strings[i][length] = '\0';

        // Невеликі модифікації
        int modifications = 1 + (i % 3); // 1-3 змін
        for (int m = 0; m < modifications; m++) {
            int pos = (i * 17 + m * 7) % length; // Псевдовипадкова позиція
            strings[i][pos] = 'a' + ((strings[i][pos] - 'a' + i + m) % 26);
        }
    }
}

// Послідовні числа як рядки
void generate_sequential_strings(char **strings, int count) {
    for (int i = 0; i < count; i++) {
        strings[i] = (char *) malloc(20);
        sprintf(strings[i], "item_%06d", i);
    }
}

// Рядки з загальними префіксами
void generate_common_prefix_strings(char **strings, int count) {
    char prefixes[][20] = {"function_", "variable_", "constant_", "method_", "class_"};
    int prefix_count = sizeof(prefixes) / sizeof(prefixes[0]);

    for (int i = 0; i < count; i++) {
        strings[i] = (char *) malloc(50);
        sprintf(strings[i], "%s%s_%d",
                prefixes[i % prefix_count],
                (i % 2) ? "long_name" : "short",
                i / prefix_count);
    }
}


void free_strings(char **strings, int count) {
    for (int i = 0; i < count; i++) {
        free(strings[i]);
    }
    free(strings);
}


void test_short_vs_long_strings() {
    printf("=== ТЕСТ: КОРОТКІ vs ДОВГІ РЯДКИ ===\n");

    const int COUNT = 100;

    // Тест коротких рядків
    char **short_strings = (char **) malloc(COUNT * sizeof(char *));
    generate_short_strings(short_strings, COUNT);

    HashQuality short_quality = analyze_distribution(short_strings, COUNT, hash,
                                                   "Короткі рядки (імена змінних)");
    print_hash_quality(short_quality);

    // Тест довгих рядків
    char **long_strings = (char **) malloc(COUNT * sizeof(char *));
    generate_long_strings(long_strings, COUNT);

    HashQuality long_quality = analyze_distribution(long_strings, COUNT, hash,
                                                  "Довгі рядки (URL-адреси)");
    print_hash_quality(long_quality);

    // Порівняння
    printf("\n=== ПОРІВНЯННЯ КОРОТКИХ vs ДОВГИХ ===\n");
    printf("Критерій               Короткі    Довгі      Переможець\n");
    printf("──────────────────────────────────────────────────────────\n");
    printf("Однорідність розподілу %.3f      %.3f      %s\n",
           short_quality.uniformity_score, long_quality.uniformity_score,
           (short_quality.uniformity_score > long_quality.uniformity_score) ? "Короткі" : "Довгі");
    printf("Макс. довжина ланцюжка %-10d %-10d %s\n",
           short_quality.max_chain_length, long_quality.max_chain_length,
           (short_quality.max_chain_length < long_quality.max_chain_length) ? "Короткі" : "Довгі");
    printf("Колізій                %-10d %-10d %s\n",
           short_quality.total_collisions, long_quality.total_collisions,
           (short_quality.total_collisions < long_quality.total_collisions) ? "Короткі" : "Довгі");

    free_strings(short_strings, COUNT);
    free_strings(long_strings, COUNT);
}

void test_similar_length_strings() {
    printf("\n=== ТЕСТ: РЯДКИ ОДНАКОВОЇ ДОВЖИНИ ===\n");

    const int COUNT = 100;
    int lengths[] = {5, 10, 15, 20};
    int length_count = sizeof(lengths) / sizeof(lengths[0]);

    printf("Тестуємо рядки різної довжини з невеликими відмінностями:\n\n");

    for (int l = 0; l < length_count; l++) {
        char **similar_strings = (char **) malloc(COUNT * sizeof(char *));
        generate_similar_strings(similar_strings, COUNT, lengths[l]);

        char description[100];
        sprintf(description, "Подібні рядки довжиною %d", lengths[l]);

        HashQuality quality = analyze_distribution(similar_strings, COUNT, hash, description);
        print_hash_quality(quality);

        // Показуємо кілька прикладів
        printf("Приклади рядків: ");
        for (int i = 0; i < 5 && i < COUNT; i++) {
            printf("'%s' ", similar_strings[i]);
        }
        printf("\n");

        free_strings(similar_strings, COUNT);
    }
}

void test_multiplier_variations() {
    printf("\n=== ТЕСТ: РІЗНІ ЗНАЧЕННЯ MULTIPLIER (31-37) ===\n");

    const int COUNT = 200;
    int multipliers[] = {31, 32, 33, 34, 35, 36, 37};
    int mult_count = sizeof(multipliers) / sizeof(multipliers[0]);

    // Використовуємо змішані дані
    char **test_strings = (char **) malloc(COUNT * sizeof(char *));
    generate_short_strings(test_strings, COUNT / 2);
    generate_long_strings(test_strings + COUNT / 2, COUNT / 2);

    printf("Тестуємо різні множники на змішаних даних (%d рядків):\n\n", COUNT);
    printf("%-10s %-12s %-12s %-15s %-12s %-10s\n",
           "Multiplier", "Однорідність", "Макс.ланцюж", "Середн.ланцюж", "Колізії", "Оцінка");
    printf("────────────────────────────────────────────────────────────────────────\n");

    HashQuality best_quality = {0};
    int best_multiplier = 31;

    for (int m = 0; m < mult_count; m++) {
        // Тимчасово змінюємо глобальний множник
        int old_multiplier = MULTIPLIER;
        MULTIPLIER = multipliers[m];

        char description[100];
        sprintf(description, "MULTIPLIER = %d", multipliers[m]);

        HashQuality quality = analyze_distribution(test_strings, COUNT, hash, description);

        printf("%-10d %-12.3f %-12d %-15.2f %-12d ",
               multipliers[m], quality.uniformity_score, quality.max_chain_length,
               quality.average_chain_length, quality.total_collisions);

        // Оцінка
        if (quality.uniformity_score >= 0.8 && quality.max_chain_length <= 3) {
            printf("ВІДМІННО\n");
        } else if (quality.uniformity_score >= 0.6 && quality.max_chain_length <= 5) {
            printf("ДОБРЕ\n");
        } else if (quality.uniformity_score >= 0.4) {
            printf("ЗАДОВІЛЬНО\n");
        } else {
            printf("ПОГАНО\n");
        }

        // Зберігаємо найкращий результат
        if (quality.uniformity_score > best_quality.uniformity_score) {
            best_quality = quality;
            best_multiplier = multipliers[m];
        }

        MULTIPLIER = old_multiplier; // Відновлюємо
    }

    printf("\nНАЙКРАЩИЙ РЕЗУЛЬТАТ: MULTIPLIER = %d\n", best_multiplier);
    printf("Однорідність: %.3f, Макс. ланцюжок: %d\n",
           best_quality.uniformity_score, best_quality.max_chain_length);

    free_strings(test_strings, COUNT);
}

void test_pathological_cases() {
    printf("\n=== ТЕСТ: ПАТОЛОГІЧНІ ВИПАДКИ ===\n");

    const int COUNT = 100;

    // Тест 1: Послідовні числа
    printf("\n--- Тест 1: Послідовні числа ---\n");
    char **sequential = (char **) malloc(COUNT * sizeof(char *));
    generate_sequential_strings(sequential, COUNT);

    HashQuality seq_quality = analyze_distribution(sequential, COUNT, hash,
                                                 "Послідовні числа");
    print_hash_quality(seq_quality);

    // Тест 2: Загальні префікси
    printf("\n--- Тест 2: Загальні префікси ---\n");
    char **common_prefix = (char **) malloc(COUNT * sizeof(char *));
    generate_common_prefix_strings(common_prefix, COUNT);

    HashQuality prefix_quality = analyze_distribution(common_prefix, COUNT, hash,
                                                    "Загальні префікси");
    print_hash_quality(prefix_quality);

    // Тест 3: Тільки цифри
    printf("\n--- Тест 3: Тільки цифри різної довжини ---\n");
    char **digit_strings = (char **) malloc(COUNT * sizeof(char *));
    for (int i = 0; i < COUNT; i++) {
        digit_strings[i] = (char *) malloc(20);
        sprintf(digit_strings[i], "%d", i * i); // Квадрати чисел
    }

    HashQuality digit_quality = analyze_distribution(digit_strings, COUNT, hash,
                                                   "Квадрати чисел");
    print_hash_quality(digit_quality);

    free_strings(sequential, COUNT);
    free_strings(common_prefix, COUNT);
    free_strings(digit_strings, COUNT);
}

void compare_hash_functions() {
    printf("\n=== ПОРІВНЯННЯ РІЗНИХ ХЕШ-ФУНКЦІЙ ===\n");

    const int COUNT = 150;

    // Створюємо змішаний набір даних
    char **mixed_data = (char **) malloc(COUNT * sizeof(char *));
    generate_short_strings(mixed_data, COUNT / 3);
    generate_long_strings(mixed_data + COUNT / 3, COUNT / 3);
    generate_similar_strings(mixed_data + 2 * COUNT / 3, COUNT / 3, 12);

    // Тестуємо стандартну хеш-функцію
    HashQuality std_quality = analyze_distribution(mixed_data, COUNT, hash,
                                                 "Стандартна (MULTIPLIER * h + c)");
    print_hash_quality(std_quality);

    // Тестуємо djb2 хеш-функцію
    HashQuality djb2_quality = analyze_distribution(mixed_data, COUNT, hash_djb2,
                                                   "DJB2 (hash * 33 + c)");
    print_hash_quality(djb2_quality);

    // Порівняння
    printf("\n=== ПІДСУМОК ПОРІВНЯННЯ ===\n");
    printf("Критерій                Стандартна   DJB2        Переможець\n");
    printf("──────────────────────────────────────────────────────────────\n");
    printf("Однорідність розподілу  %-12.3f %-12.3f %s\n",
           std_quality.uniformity_score, djb2_quality.uniformity_score,
           (std_quality.uniformity_score > djb2_quality.uniformity_score) ? "Стандартна" : "DJB2");
    printf("Макс. довжина ланцюжка  %-12d %-12d %s\n",
           std_quality.max_chain_length, djb2_quality.max_chain_length,
           (std_quality.max_chain_length < djb2_quality.max_chain_length) ? "Стандартна" : "DJB2");
    printf("Використано buckets     %-12d %-12d %s\n",
           std_quality.used_buckets, djb2_quality.used_buckets,
           (std_quality.used_buckets > djb2_quality.used_buckets) ? "Стандартна" : "DJB2");

    free_strings(mixed_data, COUNT);
}

void practical_recommendations() {
    printf("\n=== ПРАКТИЧНІ РЕКОМЕНДАЦІЇ ===\n");

    printf("НА ОСНОВІ ПРОВЕДЕНИХ ТЕСТІВ:\n\n");

    printf("1. ЯКІСТЬ ХЕШУВАННЯ КОРОТКИХ vs ДОВГИХ РЯДКІВ:\n");
    printf("   • Довгі рядки зазвичай дають кращий розподіл\n");
    printf("   • Більше інформації → краща ентропія → менше колізій\n");
    printf("   • Короткі рядки можуть утворювати кластери\n\n");

    printf("2. РЯДКИ ОДНАКОВОЇ ДОВЖИНИ:\n");
    printf("   • Невеликі відмінності можуть призводити до колізій\n");
    printf("   • Критичні позиції символів впливають на розподіл\n");
    printf("   • Довші рядки дають більше можливостей для розрізнення\n\n");

    printf("3. ОПТИМАЛЬНИЙ MULTIPLIER:\n");
    printf("   • 31 - класичний вибір (використовується в Java)\n");
    printf("   • 33 - добра альтернатива (використовується в djb2)\n");
    printf("   • Уникайте парних чисел та степенів 2\n");
    printf("   • Тестуйте на ваших реальних даних!\n\n");

    printf("4. ПАТОЛОГІЧНІ ВИПАДКИ:\n");
    printf("   • Послідовні дані можуть давати поганий розподіл\n");
    printf("   • Загальні префікси збільшують ймовірність колізій\n");
    printf("   • Розгляньте попередню обробку даних\n\n");

    printf("5. ЗАГАЛЬНІ ПОРАДИ:\n");
    printf("   • Моніторьте коефіцієнт завантаження (< 0.75)\n");
    printf("   • Відстежуйте максимальну довжину ланцюжків (< 5)\n");
    printf("   • Розгляньте збільшення таблиці при погіршенні\n");
    printf("   • Тестуйте хеш-функцію на реальних даних\n");
    printf("   • Використовуйте прості числа для розміру таблиці\n");
}

int main(void) {
    printf("=== Завдання 19: Тестування хеш-функції ===\n");

    srand(time(NULL));

    test_short_vs_long_strings();
    test_similar_length_strings();
    test_multiplier_variations();
    test_pathological_cases();
    compare_hash_functions();
    practical_recommendations();

    return 0;
}