#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
ЗАВДАННЯ 5: Видалення з перекиданням останнього елемента

Якщо порядок елементів не важливий, можна просто перемістити
останній елемент на місце видаленого. Це набагато швидше,
ніж зсув всіх елементів (O(1) замість O(n)).
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

int addname(struct NVtab *tab, Nameval newname) {
    Nameval *nvp;
    if (tab->nameval == NULL) {
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
    } else if (tab->nval >= tab->max) {
        nvp = (Nameval *) realloc(tab->nameval,
                                (NVGROW * tab->max) * sizeof(Nameval));
        if (nvp == NULL)
            return -1;
        tab->max *= NVGROW;
        tab->nameval = nvp;
    }
    tab->nameval[tab->nval] = newname;
    return tab->nval++;
}

// Оригінальна версія delname (зберігає порядок)
int delname_preserve_order(struct NVtab *tab, char *name) {
    for (int i = 0; i < tab->nval; i++) {
        if (strcmp(tab->nameval[i].name, name) == 0) {
            printf("Видаляємо '%s' з позиції %d\n", name, i);
            printf("Зсуваємо %d елементів ліворуч\n", tab->nval - i - 1);

            // Зсуваємо всі елементи після видаленого
            memmove(tab->nameval + i, tab->nameval + (i + 1),
                   (tab->nval - (i + 1)) * sizeof(Nameval));
            tab->nval--;
            return 1;
        }
    }
    return 0;
}

// НОВА версія delname (перекидає останній елемент)
int delname_swap_last(struct NVtab *tab, char *name) {
    for (int i = 0; i < tab->nval; i++) {
        if (strcmp(tab->nameval[i].name, name) == 0) {
            printf("Видаляємо '%s' з позиції %d\n", name, i);

            if (i != tab->nval - 1) {
                // Переміщуємо останній елемент на місце видаленого
                printf("Переміщуємо останній елемент ('%s') з позиції %d на позицію %d\n",
                       tab->nameval[tab->nval - 1].name, tab->nval - 1, i);
                tab->nameval[i] = tab->nameval[tab->nval - 1];
            } else {
                printf("Видаляємо останній елемент - переміщення не потрібне\n");
            }

            tab->nval--;
            return 1;
        }
    }
    return 0;
}

void print_array(struct NVtab *tab, const char *title) {
    printf("\n%s:\n", title);
    for (int i = 0; i < tab->nval; i++) {
        printf("  [%d] %s = %d\n", i, tab->nameval[i].name, tab->nameval[i].value);
    }
    printf("Кількість елементів: %d\n", tab->nval);
}

void demonstrate_preserve_order() {
    printf("=== ДЕМОНСТРАЦІЯ ЗБЕРЕЖЕННЯ ПОРЯДКУ ===\n");

    // Ініціалізація
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Додаємо тестові дані
    char names[][10] = {"Alice", "Bob", "Charlie", "David", "Eve"};
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        Nameval item = {names[i], values[i]};
        addname(&nvtab, item);
    }

    print_array(&nvtab, "Початковий масив");

    // Видаляємо елемент з середини
    printf("\n--- Видаляємо 'Charlie' (зберігаючи порядок) ---\n");
    delname_preserve_order(&nvtab, "Charlie");
    print_array(&nvtab, "Після видалення 'Charlie'");

    printf("Порядок збережено: Alice, Bob, David, Eve\n");
}

void demonstrate_swap_last() {
    printf("\n=== ДЕМОНСТРАЦІЯ ПЕРЕКИДАННЯ ОСТАННЬОГО ===\n");

    // Скидаємо масив
    free(nvtab.nameval);
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Додаємо тестові дані
    char names[][10] = {"Alice", "Bob", "Charlie", "David", "Eve"};
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        Nameval item = {names[i], values[i]};
        addname(&nvtab, item);
    }

    print_array(&nvtab, "Початковий масив");

    // Видаляємо елемент з середини (швидкий спосіб)
    printf("\n--- Видаляємо 'Charlie' (перекидаючи останній) ---\n");
    delname_swap_last(&nvtab, "Charlie");
    print_array(&nvtab, "Після видалення 'Charlie'");

    printf("Порядок змінено: Alice, Bob, Eve, David\n");
    printf("'Eve' перемістилася на місце 'Charlie'\n");
}

void demonstrate_performance_difference() {
    printf("\n=== ПОРІВНЯННЯ ПРОДУКТИВНОСТІ ===\n");

    const int ARRAY_SIZE = 1000;

    // Тест 1: Збереження порядку
    free(nvtab.nameval);
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    printf("Створюємо масив з %d елементів для тесту збереження порядку...\n",
           ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        char name[20];
        sprintf(name, "Item%d", i);
        Nameval item = {strdup(name), i};
        addname(&nvtab, item);
    }

    printf("Видаляємо елемент з середини (індекс 500):\n");
    printf("Операція збереження порядку: ");
    printf("потрібно зсунути %d елементів\n", ARRAY_SIZE - 500 - 1);

    // Тест 2: Перекидання останнього
    printf("\nОперація перекидання останнього: ");
    printf("потрібно перемістити 1 елемент\n");

    printf("\nСкладність:\n");
    printf("- Збереження порядку: O(n) - лінійна\n");
    printf("- Перекидання останнього: O(1) - константна\n");

    // Звільняємо динамічну пам'ять
    for (int i = 0; i < nvtab.nval; i++) {
        free(nvtab.nameval[i].name);
    }
}

void demonstrate_edge_cases() {
    printf("\n=== КРАЙНІ ВИПАДКИ ===\n");

    free(nvtab.nameval);
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Один елемент
    Nameval single = {"OnlyOne", 42};
    addname(&nvtab, single);
    print_array(&nvtab, "Масив з одним елементом");

    printf("\n--- Видаляємо єдиний елемент ---\n");
    delname_swap_last(&nvtab, "OnlyOne");
    print_array(&nvtab, "Після видалення єдиного елемента");

    // Два елементи
    Nameval first = {"First", 1};
    Nameval second = {"Second", 2};
    addname(&nvtab, first);
    addname(&nvtab, second);
    print_array(&nvtab, "Масив з двома елементами");

    printf("\n--- Видаляємо перший елемент ---\n");
    delname_swap_last(&nvtab, "First");
    print_array(&nvtab, "Після видалення першого");

    printf("\n--- Видаляємо останній елемент ---\n");
    delname_swap_last(&nvtab, "Second");
    print_array(&nvtab, "Після видалення останнього");
}

int main(void) {
    printf("=== Завдання 5: Перекидання останнього елемента ===\n\n");

    demonstrate_preserve_order();
    demonstrate_swap_last();
    demonstrate_performance_difference();
    demonstrate_edge_cases();

    printf("\n=== ВИСНОВКИ ===\n");
    printf("Переваги перекидання останнього елемента:\n");
    printf("+ Швидкість: O(1) замість O(n)\n");
    printf("+ Простота реалізації\n");
    printf("+ Менше операцій з пам'яттю\n\n");

    printf("Недоліки:\n");
    printf("- Порядок елементів не зберігається\n");
    printf("- Не підходить, якщо порядок важливий\n\n");

    printf("Використовуйте цей метод, коли:\n");
    printf("- Порядок елементів не важливий\n");
    printf("- Потрібна максимальна швидкість видалення\n");
    printf("- Часто виконуються операції видалення\n");

    free(nvtab.nameval);
    return 0;
}