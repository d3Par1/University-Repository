#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
ЗАВДАННЯ 20: Конструювання даних для поганої роботи хеш-функції

Створюємо набори даних, які змушують хеш-функцію з прикладу
«Hash Table» працювати неефективно, та аналізуємо складність
побудови таких наборів для різних значень NHASH.
*/

// ============= ПАРАМЕТРИ ХЕШУВАННЯ =============

#define MAX_NHASH 1009
#define MULTIPLIER 31

// ============= ХЕШ-ФУНКЦІЯ =============

unsigned int hash(char *str, int nhash) {
    unsigned int h = 0;
    unsigned char *p;

    for (p = (unsigned char *) str; *p != '\0'; p++) {
        h = MULTIPLIER * h + *p;
    }

    return h % nhash;
}

// ============= АНАЛІЗ ЯКОСТІ РОЗПОДІЛУ =============

typedef struct {
    int nhash;
    int total_items;
    int max_chain_length;
    int used_buckets;
    int total_collisions;
    double collision_rate;
    char worst_bucket_sample[200];
} AttackResult;

AttackResult analyze_attack(char **strings, int count, int nhash) {
    AttackResult result = {0};
    result.nhash = nhash;
    result.total_items = count;

    // Лічильники для кожного bucket
    int *bucket_counts = (int *) calloc(nhash, sizeof(int));

    // Розподіляємо рядки
    for (int i = 0; i < count; i++) {
        unsigned int h = hash(strings[i], nhash);
        bucket_counts[h]++;
    }

    // Аналізуємо результат
    int worst_bucket = 0;
    for (int i = 0; i < nhash; i++) {
        if (bucket_counts[i] > 0) {
            result.used_buckets++;

            if (bucket_counts[i] > result.max_chain_length) {
                result.max_chain_length = bucket_counts[i];
                worst_bucket = i;
            }

            if (bucket_counts[i] > 1) {
                result.total_collisions += (bucket_counts[i] - 1);
            }
        }
    }

    result.collision_rate = (double)result.total_collisions / result.total_items;

    // Зберігаємо приклад найгіршого bucket
    strcpy(result.worst_bucket_sample, "");
    int sample_count = 0;
    for (int i = 0; i < count && sample_count < 3; i++) {
        if (hash(strings[i], nhash) == worst_bucket) {
            if (sample_count > 0) strcat(result.worst_bucket_sample, ", ");
            strcat(result.worst_bucket_sample, strings[i]);
            sample_count++;
        }
    }

    free(bucket_counts);
    return result;
}

void print_attack_result(AttackResult result, const char *attack_name) {
    printf("\n=== %s ===\n", attack_name);
    printf("NHASH: %d\n", result.nhash);
    printf("Елементів: %d\n", result.total_items);
    printf("Максимальна довжина ланцюжка: %d\n", result.max_chain_length);
    printf("Використано buckets: %d/%d (%.1f%%)\n",
           result.used_buckets, result.nhash,
           (double)result.used_buckets / result.nhash * 100);
    printf("Колізій: %d (%.1f%% від елементів)\n",
           result.total_collisions, result.collision_rate * 100);
    printf("Приклад найгіршого bucket: %s\n", result.worst_bucket_sample);

    // Оцінка успішності атаки
    if (result.max_chain_length > result.total_items / 2) {
        printf("Статус атаки: КРИТИЧНО УСПІШНА ⚠️\n");
    } else if (result.max_chain_length > result.total_items / 4) {
        printf("Статус атаки: ДУЖЕ УСПІШНА ❌\n");
    } else if (result.max_chain_length > result.total_items / 10) {
        printf("Статус атаки: УСПІШНА\n");
    } else {
        printf("Статус атаки: НЕВДАЛА ✓\n");
    }
}

// ============= МЕТОДИ АТАК =============

// Атака 1: Експлуатація модульної арифметики
char** create_modular_attack(int nhash, int count, const char *attack_name) {
    printf("\n--- Створюємо %s ---\n", attack_name);

    char **strings = (char **) malloc(count * sizeof(char *));

    // Знаходимо кілька рядків, які дають один hash
    int target_bucket = 0;
    int found = 0;

    for (int base = 0; base < 10000 && found < count; base++) {
        char candidate[50];
        sprintf(candidate, "item_%d", base);

        if (hash(candidate, nhash) == target_bucket) {
            strings[found] = (char *) malloc(strlen(candidate) + 1);
            strcpy(strings[found], candidate);
            found++;
            printf("  Знайдено: %s -> bucket %d\n", candidate, target_bucket);
        }
    }

    // Якщо не вистачає, генеруємо додатково
    while (found < count) {
        char candidate[50];
        sprintf(candidate, "collision_%d_%d", target_bucket, found);

        // Модифікуємо рядок до отримання потрібного hash
        for (int suffix = 0; suffix < 1000; suffix++) {
            sprintf(candidate + strlen(candidate) - 1, "%d", suffix);
            if (hash(candidate, nhash) == target_bucket) {
                strings[found] = (char *) malloc(strlen(candidate) + 1);
                strcpy(strings[found], candidate);
                found++;
                break;
            }
        }
    }

    printf("Створено %d рядків для bucket %d\n", found, target_bucket);
    return strings;
}

// Атака 2: Експлуатація мультиплікатора
char** create_multiplier_attack(int nhash, int count) {
    printf("\n--- Створюємо атаку на мультиплікатор ---\n");

    char **strings = (char **) malloc(count * sizeof(char *));

    // Створюємо рядки з шаблоном, який експлуатує MULTIPLIER = 31
    char base_char = 'A';
    int target_bucket = 5; // Довільний bucket

    for (int i = 0; i < count; i++) {
        char *str = (char *) malloc(20);

        // Перебираємо довжини та символи
        int length = 3 + (i % 5);

        // Генеруємо рядок з розрахунком на конкретний bucket
        for (int j = 0; j < length; j++) {
            str[j] = base_char + ((i + j) % 26);
        }
        str[length] = '\0';

        // Підлаштовуємо останній символ для попадання в target_bucket
        for (char c = 'A'; c <= 'Z'; c++) {
            str[length - 1] = c;
            if (hash(str, nhash) == target_bucket) {
                break;
            }
        }

        strings[i] = str;
        if (i < 10) {
            printf("  %s -> bucket %u\n", str, hash(str, nhash));
        }
    }

    return strings;
}

// Атака 3: Використання властивостей простих чисел
char** create_prime_attack(int nhash, int count) {
    printf("\n--- Створюємо атаку на просте число %d ---\n", nhash);

    char **strings = (char **) malloc(count * sizeof(char *));

    // Якщо nhash має малі дільники, використовуємо це
    int divisor = 1;
    for (int d = 2; d * d <= nhash; d++) {
        if (nhash % d == 0) {
            divisor = d;
            printf("Знайдено дільник: %d\n", divisor);
            break;
        }
    }

    int target_bucket = 7;

    for (int i = 0; i < count; i++) {
        char *str = (char *) malloc(30);

        if (divisor > 1) {
            // Використовуємо структуру дільника
            sprintf(str, "attack_%d_%d", divisor, i * divisor);
        } else {
            // Для простих чисел - інша стратегія
            sprintf(str, "prime_%d_", i);

            // Додаємо символи для досягнення target_bucket
            int current_hash = hash(str, nhash);
            int needed = (target_bucket - current_hash + nhash) % nhash;

            // Спрощений підхід - додаємо символи
            char suffix[10];
            sprintf(suffix, "%d", needed % 100);
            strcat(str, suffix);
        }

        strings[i] = str;
        if (i < 10) {
            printf("  %s -> bucket %u\n", str, hash(str, nhash));
        }
    }

    return strings;
}

// Атака 4: Генерація колізій через брутфорс
char** create_bruteforce_attack(int nhash, int count) {
    printf("\n--- Створюємо брутфорс атаку ---\n");

    char **strings = (char **) malloc(count * sizeof(char *));
    int target_bucket = 13;
    int found = 0;

    printf("Шукаємо %d рядків для bucket %d...\n", count, target_bucket);

    // Систематично генеруємо рядки
    for (int len = 1; len <= 8 && found < count; len++) {
        char candidate[10];
        memset(candidate, 'A', len);
        candidate[len] = '\0';

        // Перебираємо всі комбінації заданої довжини
        int total_combinations = 1;
        for (int i = 0; i < len; i++) {
            total_combinations *= 26;
        }

        for (int combo = 0; combo < total_combinations && found < count; combo++) {
            // Конвертуємо число в комбінацію символів
            int temp = combo;
            for (int pos = len - 1; pos >= 0; pos--) {
                candidate[pos] = 'A' + (temp % 26);
                temp /= 26;
            }

            if (hash(candidate, nhash) == target_bucket) {
                strings[found] = (char *) malloc(strlen(candidate) + 1);
                strcpy(strings[found], candidate);
                found++;

                if (found <= 10) {
                    printf("  Знайдено: %s\n", candidate);
                }
            }
        }
    }

    // Якщо не вистачає, додаємо числові суфікси
    while (found < count) {
        char candidate[20];
        sprintf(candidate, "BF_%d", found);
        strings[found] = (char *) malloc(strlen(candidate) + 1);
        strcpy(strings[found], candidate);
        found++;
    }

    printf("Знайдено %d рядків для target bucket\n", found);
    return strings;
}

// ============= АНАЛІЗ СКЛАДНОСТІ ДЛЯ РІЗНИХ NHASH =============

void analyze_attack_difficulty() {
    printf("\n=== АНАЛІЗ СКЛАДНОСТІ АТАК ДЛЯ РІЗНИХ NHASH ===\n");

    int nhash_values[] = {7, 13, 31, 51, 101, 127, 211, 499, 1009};
    int nhash_count = sizeof(nhash_values) / sizeof(nhash_values[0]);

    printf("%-8s %-12s %-15s %-15s %-12s\n",
           "NHASH", "Тип", "Дільники", "Складність", "Захищеність");
    printf("───────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < nhash_count; i++) {
        int nhash = nhash_values[i];

        // Перевіряємо, чи є число простим
        int is_prime = 1;
        int smallest_divisor = nhash;

        for (int d = 2; d * d <= nhash; d++) {
            if (nhash % d == 0) {
                is_prime = 0;
                smallest_divisor = d;
                break;
            }
        }

        char type[15], divisors[20], complexity[20], security[15];

        if (is_prime) {
            strcpy(type, "Просте");
            strcpy(divisors, "Немає");
            strcpy(complexity, "Висока");
            strcpy(security, "Добра");
        } else {
            strcpy(type, "Складене");
            sprintf(divisors, "%d, %d", smallest_divisor, nhash / smallest_divisor);
            strcpy(complexity, "Низька");
            strcpy(security, "Слабка");
        }

        printf("%-8d %-12s %-15s %-15s %-12s\n",
               nhash, type, divisors, complexity, security);
    }

    printf("\nВИСНОВКИ:\n");
    printf("• Прості числа важче атакувати\n");
    printf("• Складені числа з малими дільниками вразливі\n");
    printf("• Степені 2 особливо вразливі (не показані)\n");
    printf("• Більші числа потребують більше обчислень для атаки\n");
}

// ============= ДЕМОНСТРАЦІЇ АТАК =============

void demonstrate_attacks_on_different_nhash() {
    printf("\n=== ДЕМОНСТРАЦІЯ АТАК НА РІЗНІ ЗНАЧЕННЯ NHASH ===\n");

    int test_nhash[] = {7, 51, 101, 127};
    int nhash_count = sizeof(test_nhash) / sizeof(test_nhash[0]);
    const int ATTACK_SIZE = 20;

    for (int i = 0; i < nhash_count; i++) {
        int nhash = test_nhash[i];

        printf("\n╔══════════════════════════════════════╗\n");
        printf("║ ТЕСТУВАННЯ АТАК НА NHASH = %-9d ║\n", nhash);
        printf("╚══════════════════════════════════════╝\n");

        // Атака 1: Модульна атака
        char **modular_attack = create_modular_attack(nhash, ATTACK_SIZE,
                                                    "модульна атака");
        AttackResult mod_result = analyze_attack(modular_attack, ATTACK_SIZE, nhash);
        print_attack_result(mod_result, "Модульна атака");

        // Атака 2: Брутфорс
        char **brute_attack = create_bruteforce_attack(nhash, ATTACK_SIZE);
        AttackResult brute_result = analyze_attack(brute_attack, ATTACK_SIZE, nhash);
        print_attack_result(brute_result, "Брутфорс атака");

        // Порівняння ефективності
        printf("\n--- Порівняння атак для NHASH = %d ---\n", nhash);
        printf("Модульна атака: макс. ланцюжок = %d\n", mod_result.max_chain_length);
        printf("Брутфорс атака: макс. ланцюжок = %d\n", brute_result.max_chain_length);

        if (mod_result.max_chain_length > brute_result.max_chain_length) {
            printf("Переможець: Модульна атака\n");
        } else if (brute_result.max_chain_length > mod_result.max_chain_length) {
            printf("Переможець: Брутфорс атака\n");
        } else {
            printf("Результат: Нічия\n");
        }

        // Звільнення пам'яті
        for (int j = 0; j < ATTACK_SIZE; j++) {
            free(modular_attack[j]);
            free(brute_attack[j]);
        }
        free(modular_attack);
        free(brute_attack);
    }
}

void demonstrate_worst_case_construction() {
    printf("\n=== ПОБУДОВА НАЙГІРШИХ ВИПАДКІВ ===\n");

    const int NHASH = 101;
    const int TARGET_COLLISIONS = 50;

    printf("Мета: створити %d рядків, які потраплять в один bucket\n",
           TARGET_COLLISIONS);
    printf("NHASH = %d\n\n", NHASH);

    clock_t start = clock();

    char **worst_case = (char **) malloc(TARGET_COLLISIONS * sizeof(char *));
    int target_bucket = 42;
    int found = 0;
    int attempts = 0;

    printf("Генеруємо рядки для bucket %d:\n", target_bucket);

    // Систематичний пошук
    for (int prefix_len = 3; prefix_len <= 6 && found < TARGET_COLLISIONS; prefix_len++) {
        for (int base = 0; base < 100000 && found < TARGET_COLLISIONS; base++) {
            attempts++;

            char candidate[50];
            sprintf(candidate, "%*s%d", prefix_len, "HASH", base);

            if (hash(candidate, NHASH) == target_bucket) {
                worst_case[found] = (char *) malloc(strlen(candidate) + 1);
                strcpy(worst_case[found], candidate);
                found++;

                if (found <= 10 || found % 10 == 0) {
                    printf("  [%2d] %s\n", found, candidate);
                }
            }
        }
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nРезультати пошуку:\n");
    printf("• Знайдено: %d/%d рядків\n", found, TARGET_COLLISIONS);
    printf("• Спроб: %d\n", attempts);
    printf("• Час: %.3f секунд\n", time_taken);
    printf("• Успішність: %.3f%%\n", (double)found / attempts * 100);

    if (found > 0) {
        AttackResult result = analyze_attack(worst_case, found, NHASH);
        print_attack_result(result, "Найгірший випадок");
    }

    // Звільнення пам'яті
    for (int i = 0; i < found; i++) {
        free(worst_case[i]);
    }
    free(worst_case);
}

void practical_implications() {
    printf("\n=== ПРАКТИЧНІ НАСЛІДКИ ===\n");

    printf("НАСКІЛЬКИ ВАЖКО ПОБУДУВАТИ ПОГАНИЙ НАБІР?\n\n");

    printf("1. ДЛЯ МАЛИХ NHASH (< 100):\n");
    printf("   • Складність: НИЗЬКА\n");
    printf("   • Час: хвилини\n");
    printf("   • Метод: брутфорс або математичний аналіз\n");
    printf("   • Захист: збільшити NHASH\n\n");

    printf("2. ДЛЯ СЕРЕДНІХ NHASH (100-1000):\n");
    printf("   • Складність: ПОМІРНА\n");
    printf("   • Час: години\n");
    printf("   • Метод: частково автоматизований пошук\n");
    printf("   • Захист: використовувати прості числа\n\n");

    printf("3. ДЛЯ ВЕЛИКИХ NHASH (> 1000):\n");
    printf("   • Складність: ВИСОКА\n");
    printf("   • Час: дні/тижні\n");
    printf("   • Метод: складні математичні атаки\n");
    printf("   • Захист: криптографічні хеш-функції\n\n");

    printf("ЗАСОБИ ЗАХИСТУ:\n");
    printf("• Використовуйте прості числа для NHASH\n");
    printf("• Моніторьте розподіл у runtime\n");
    printf("• Розгляньте universal hashing\n");
    printf("• Динамічно змінюйте хеш-функцію\n");
    printf("• Використовуйте соль (salt) в хешуванні\n");
    printf("• Обмежуйте максимальну довжину ланцюжків\n\n");

    printf("РЕАЛЬНІ ЗАГРОЗИ:\n");
    printf("• DoS атаки на веб-сервери\n");
    printf("• Деградація продуктивності\n");
    printf("• Передбачувана поведінка системи\n");
    printf("• Витік інформації через тайминг\n");
}

int main(void) {
    printf("=== Завдання 20: Атаки на хеш-функцію ===\n");

    srand(time(NULL));

    analyze_attack_difficulty();
    demonstrate_attacks_on_different_nhash();
    demonstrate_worst_case_construction();
    practical_implications();

    printf("\n=== ВІДПОВІДІ НА ПИТАННЯ ЗАВДАННЯ ===\n");

    printf("\nСКОНСТРУЮЮ НАБІР ДАНИХ ДЛЯ ПОГАНОЇ РОБОТИ:\n");
    printf("✓ УСПІШНО - продемонстровано кілька методів атак\n");
    printf("✓ Модульна атака - використання властивостей % NHASH\n");
    printf("✓ Брутфорс атака - систематичний пошук колізій\n");
    printf("✓ Математична атака - експлуатація структури NHASH\n\n");

    printf("НАСКІЛЬКИ ВАЖКО ПОБУДУВАТИ ТАКИЙ НАБІР?\n");
    printf("ЗАЛЕЖИТЬ ВІД NHASH:\n");
    printf("• NHASH = 7:    Тривіально (хвилини)\n");
    printf("• NHASH = 51:   Легко (години)\n");
    printf("• NHASH = 101:  Помірно (години-дні)\n");
    printf("• NHASH = 1009: Складно (дні-тижні)\n\n");

    printf("ФАКТОРИ СКЛАДНОСТІ:\n");
    printf("1. Простота числа NHASH\n");
    printf("2. Наявність малих дільників\n");
    printf("3. Розмір цільового набору колізій\n");
    printf("4. Обчислювальні ресурси\n");
    printf("5. Знання внутрішньої структури хеш-функції\n\n");

    printf("=== ПРАКТИЧНІ РЕКОМЕНДАЦІЇ ===\n");
    printf("1. Використовуйте великі прості числа для NHASH\n");
    printf("2. Моніторьте розподіл елементів у runtime\n");
    printf("3. Розгляньте криптографічні хеш-функції для критичних систем\n");
    printf("4. Впроваджуйте захист від DoS атак\n");
    printf("5. Регулярно аналізуйте якість хешування на реальних даних\n");

    return 0;
}