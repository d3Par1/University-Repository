#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
ЗАВДАННЯ 4: Проблема з конструкцією
nvtab.nameval = (Nameval *) realloc(nvtab.nameval, ...)

ПРОБЛЕМА:
Якщо realloc не може виділити пам'ять, він повертає NULL.
При прямому присвоєнні nvtab.nameval = realloc(...) ми втрачаємо
посилання на оригінальний блок пам'яті, що призводить до втрати всіх даних!

ПРАВИЛЬНО: спочатку зберегти результат realloc в тимчасовій змінній,
перевірити на NULL, і тільки потім присвоїти.
*/

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
};

struct NVtab {
    int nval;
    int max;
    Nameval *nameval;
} nvtab;

enum {NVINIT = 1, NVGROW = 2};

// НЕБЕЗПЕЧНА версія addname (втрачає дані при помилці realloc)
int addname_dangerous(struct NVtab *tab, Nameval newname) {
    if (tab->nameval == NULL) {
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
    } else if (tab->nval >= tab->max) {
        // НЕБЕЗПЕЧНО! При помилці втратимо всі дані
        tab->nameval = (Nameval *) realloc(tab->nameval,
                                         (NVGROW * tab->max) * sizeof(Nameval));
        if (tab->nameval == NULL) {
            printf("ПОМИЛКА: realloc повернув NULL!\n");
            printf("ВСІ ДАНІ ВТРАЧЕНО! Оригінальний масив більше недоступний!\n");
            return -1;
        }
        tab->max *= NVGROW;
    }
    tab->nameval[tab->nval] = newname;
    return tab->nval++;
}

// БЕЗПЕЧНА версія addname
int addname_safe(struct NVtab *tab, Nameval newname) {
    Nameval *nvp;
    if (tab->nameval == NULL) {
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
    } else if (tab->nval >= tab->max) {
        // БЕЗПЕЧНО! Зберігаємо результат в тимчасовій змінній
        nvp = (Nameval *) realloc(tab->nameval,
                                (NVGROW * tab->max) * sizeof(Nameval));
        if (nvp == NULL) {
            printf("ПОМИЛКА: realloc повернув NULL!\n");
            printf("Але оригінальні дані збережені! tab->nameval все ще дійсний.\n");
            return -1; // Оригінальні дані залишаються доступними
        }
        tab->max *= NVGROW;
        tab->nameval = nvp; // Присвоюємо тільки після перевірки
    }
    tab->nameval[tab->nval] = newname;
    return tab->nval++;
}

// Функція для симуляції помилки realloc
void* failing_realloc(void* ptr, size_t size) {
    static int call_count = 0;
    call_count++;

    // Симулюємо помилку на 3-му виклику
    if (call_count == 3) {
        printf("Симуляція помилки realloc (повертаємо NULL)\n");
        errno = ENOMEM;
        return NULL;
    }

    return realloc(ptr, size);
}

// Версія з симуляцією помилки для демонстрації
int addname_with_simulated_failure(struct NVtab *tab, Nameval newname) {
    Nameval *nvp;
    if (tab->nameval == NULL) {
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
    } else if (tab->nval >= tab->max) {
        printf("Спроба realloc для розширення з %d до %d елементів\n",
               tab->max, NVGROW * tab->max);

        // Використовуємо функцію з симуляцією помилки
        nvp = (Nameval *) failing_realloc(tab->nameval,
                                        (NVGROW * tab->max) * sizeof(Nameval));
        if (nvp == NULL) {
            printf("realloc FAILED! Але дані збережені.\n");
            return -1;
        }
        tab->max *= NVGROW;
        tab->nameval = nvp;
        printf("realloc успішний. Новий розмір: %d\n", tab->max);
    }
    tab->nameval[tab->nval] = newname;
    return tab->nval++;
}

void demonstrate_data_loss() {
    printf("=== ДЕМОНСТРАЦІЯ ВТРАТИ ДАНИХ ===\n");

    // Створюємо тестові дані
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    printf("Додаємо початкові дані:\n");
    Nameval items[] = {
        {"Important1", 100},
        {"Important2", 200}
    };

    for (int i = 0; i < 2; i++) {
        if (addname_with_simulated_failure(&nvtab, items[i]) >= 0) {
            printf("Додано: %s = %d\n", items[i].name, items[i].value);
        }
    }

    printf("\nПоточні дані в масиві:\n");
    for (int i = 0; i < nvtab.nval; i++) {
        printf("  %s = %d\n", nvtab.nameval[i].name, nvtab.nameval[i].value);
    }

    // Спробуємо додати третій елемент (спричинить помилку realloc)
    printf("\nСпробуємо додати третій елемент (спричинить помилку):\n");
    Nameval item3 = {"WillFail", 300};

    if (addname_with_simulated_failure(&nvtab, item3) < 0) {
        printf("\nПісля помилки realloc:\n");
        if (nvtab.nameval != NULL) {
            printf("Дані ВСЕ ЩЕ ДОСТУПНІ:\n");
            for (int i = 0; i < nvtab.nval; i++) {
                printf("  %s = %d\n", nvtab.nameval[i].name, nvtab.nameval[i].value);
            }
        } else {
            printf("ДАНІ ВТРАЧЕНО! nvtab.nameval == NULL\n");
        }
    }
}

void demonstrate_correct_approach() {
    printf("\n=== ДЕМОНСТРАЦІЯ ПРАВИЛЬНОГО ПІДХОДУ ===\n");

    // Приклад коду з правильною обробкою помилок
    printf("Правильний код:\n");
    printf("nvp = (Nameval *) realloc(tab->nameval, new_size);\n");
    printf("if (nvp == NULL) {\n");
    printf("    // Помилка! Але tab->nameval все ще дійсний\n");
    printf("    return -1;\n");
    printf("}\n");
    printf("tab->nameval = nvp; // Присвоюємо тільки після перевірки\n\n");

    printf("Неправильний код:\n");
    printf("tab->nameval = (Nameval *) realloc(tab->nameval, new_size);\n");
    printf("if (tab->nameval == NULL) {\n");
    printf("    // Помилка! І оригінальні дані втрачено!\n");
    printf("    return -1;\n");
    printf("}\n");
}

void demonstrate_memory_exhaustion() {
    printf("\n=== ДЕМОНСТРАЦІЯ ВИЧЕРПАННЯ ПАМ'ЯТІ ===\n");

    struct NVtab test_tab = {0, 0, NULL};

    printf("Спробуємо виділити занадто багато пам'яті:\n");

    // Спробуємо виділити 1GB (щоб гарантовано отримати помилку)
    size_t huge_size = 1024 * 1024 * 1024; // 1GB

    Nameval *huge_ptr = (Nameval *) realloc(test_tab.nameval, huge_size);
    if (huge_ptr == NULL) {
        printf("realloc не зміг виділити %zu байт\n", huge_size);
        printf("errno: %s\n", strerror(errno));
        printf("test_tab.nameval все ще: %p (дані збережені)\n",
               (void*)test_tab.nameval);
    } else {
        printf("Несподівано: realloc успішний!\n");
        free(huge_ptr);
    }
}

int main(void) {
    printf("=== Завдання 4: Проблема з realloc ===\n\n");

    demonstrate_data_loss();
    demonstrate_correct_approach();
    demonstrate_memory_exhaustion();

    printf("\n=== ВИСНОВКИ ===\n");
    printf("1. НІКОЛИ не присвоюйте результат realloc безпосередньо!\n");
    printf("2. Завжди використовуйте тимчасову змінну для перевірки\n");
    printf("3. При помилці realloc оригінальний блок залишається дійсним\n");
    printf("4. Правильна обробка помилок зберігає ваші дані\n");

    if (nvtab.nameval != NULL) {
        free(nvtab.nameval);
    }

    return 0;
}