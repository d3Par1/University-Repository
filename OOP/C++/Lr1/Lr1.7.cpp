#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
ЗАВДАННЯ 7: Функція для виведення всіх елементів односв'язного списку

Базуючись на прикладі "A singly-linked list" з лекції,
реалізуємо функції для перебору та виведення елементів списку.
*/

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;  // вказівник на наступний елемент
};

// Глобальний вказівник на початок списку
Nameval *nvlist = NULL;

// Створення нового елемента (допоміжна функція)
Nameval* newitem(char *name, int value) {
    Nameval *newp;

    newp = (Nameval *) malloc(sizeof(Nameval));
    if (newp == NULL)
        return NULL;

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

// Пошук елемента в списку
Nameval* lookup(Nameval *listp, char *name) {
    for (Nameval *p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0)
            return p;
    }
    return NULL; // не знайдено
}

// ЗАВДАННЯ 7.1: Функція для обчислення довжини списку
int list_length(Nameval *listp) {
    int count = 0;
    for (Nameval *p = listp; p != NULL; p = p->next) {
        count++;
    }
    return count;
}

// ЗАВДАННЯ 7.2: Функція для виведення всіх елементів списку
void print_list(Nameval *listp) {
    if (listp == NULL) {
        printf("Список порожній\n");
        return;
    }

    printf("Елементи списку:\n");
    int index = 0;
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("  [%d] %s = %d\n", index++, p->name, p->value);
    }
    printf("Загальна кількість: %d елементів\n", index);
}

// ЗАВДАННЯ 7.3: Функція для виведення з додатковою інформацією
void print_list_detailed(Nameval *listp, const char *title) {
    printf("\n=== %s ===\n", title);

    if (listp == NULL) {
        printf("Список порожній (listp = NULL)\n");
        return;
    }

    int length = list_length(listp);
    printf("Довжина списку: %d\n", length);
    printf("Початок списку: %p\n", (void*)listp);

    printf("\nДетальна інформація:\n");
    int index = 0;
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("  [%d] Адреса: %p\n", index, (void*)p);
        printf("      Ім'я: %s\n", p->name);
        printf("      Значення: %d\n", p->value);
        printf("      Наступний: %p\n", (void*)p->next);
        if (p->next == NULL) {
            printf("      (кінець списку)\n");
        }
        printf("\n");
        index++;
    }
}

// ЗАВДАННЯ 7.4: Функція для виведення в зворотному порядку (рекурсивно)
void print_list_reverse(Nameval *listp) {
    if (listp == NULL) {
        return;
    }

    // Спочатку виводимо решту списку
    print_list_reverse(listp->next);

    // Потім виводимо поточний елемент
    printf("  %s = %d\n", listp->name, listp->value);
}

// Функція для звільнення всієї пам'яті списку
void free_list(Nameval *listp) {
    Nameval *next;
    for (Nameval *p = listp; p != NULL; p = next) {
        next = p->next;
        free(p->name);
        free(p);
    }
}

// Функція для демонстрації різних способів обходу
void demonstrate_traversal_methods(Nameval *listp) {
    printf("\n=== РІЗНІ СПОСОБИ ОБХОДУ СПИСКУ ===\n");

    // Спосіб 1: Простий перебір
    printf("\n1. Простий перебір:\n");
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("  %s = %d\n", p->name, p->value);
    }

    // Спосіб 2: З індексами
    printf("\n2. З індексами:\n");
    int i = 0;
    for (Nameval *p = listp; p != NULL; p = p->next, i++) {
        printf("  [%d] %s = %d\n", i, p->name, p->value);
    }

    // Спосіб 3: З перевіркою на останній елемент
    printf("\n3. З позначенням останнього:\n");
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("  %s = %d", p->name, p->value);
        if (p->next == NULL) {
            printf(" (останній)");
        }
        printf("\n");
    }

    // Спосіб 4: Зворотний порядок
    printf("\n4. Зворотний порядок (рекурсивно):\n");
    print_list_reverse(listp);
}

int main(void) {
    printf("=== Завдання 7: Виведення елементів односв'язного списку ===\n");

    // Створюємо тестові дані
    printf("\nСтворюємо список:\n");

    // Додаємо елементи (в зворотному порядку, бо додаємо на початок)
    nvlist = addfront(nvlist, newitem("Eve", 50));
    printf("Додано: Eve = 50\n");

    nvlist = addfront(nvlist, newitem("David", 40));
    printf("Додано: David = 40\n");

    nvlist = addfront(nvlist, newitem("Charlie", 30));
    printf("Додано: Charlie = 30\n");

    nvlist = addfront(nvlist, newitem("Bob", 20));
    printf("Додано: Bob = 20\n");

    nvlist = addfront(nvlist, newitem("Alice", 10));
    printf("Додано: Alice = 10\n");

    // Демонстрація різних функцій виведення
    print_list(nvlist);

    print_list_detailed(nvlist, "Детальна інформація про список");

    demonstrate_traversal_methods(nvlist);

    // Тестування функції довжини
    printf("\n=== ТЕСТУВАННЯ ДОВЖИНИ ===\n");
    printf("Довжина списку: %d\n", list_length(nvlist));

    // Тестування пошуку
    printf("\n=== ТЕСТУВАННЯ ПОШУКУ ===\n");
    char search_names[][10] = {"Bob", "Charlie", "Unknown"};

    for (int i = 0; i < 3; i++) {
        Nameval *found = lookup(nvlist, search_names[i]);
        if (found != NULL) {
            printf("Знайдено: %s = %d (адреса: %p)\n",
                   found->name, found->value, (void*)found);
        } else {
            printf("Не знайдено: %s\n", search_names[i]);
        }
    }

    // Тестування з порожнім списком
    printf("\n=== ТЕСТУВАННЯ З ПОРОЖНІМ СПИСКОМ ===\n");
    print_list(NULL);
    print_list_detailed(NULL, "Порожній список");
    printf("Довжина порожнього списку: %d\n", list_length(NULL));

    // Демонстрація поетапного видалення
    printf("\n=== ПОЕТАПНЕ СКОРОЧЕННЯ СПИСКУ ===\n");
    Nameval *current = nvlist;
    int step = 1;

    while (current != NULL) {
        printf("\nКрок %d - видаляємо перший елемент:\n", step++);
        Nameval *to_delete = current;
        current = current->next;

        printf("Видаляємо: %s\n", to_delete->name);
        free(to_delete->name);
        free(to_delete);

        printf("Залишилося:\n");
        print_list(current);
    }

    printf("\n=== ВИСНОВКИ ===\n");
    printf("Реалізовано функції:\n");
    printf("1. list_length() - обчислення довжини списку O(n)\n");
    printf("2. print_list() - простий вивід всіх елементів\n");
    printf("3. print_list_detailed() - детальний вивід з адресами\n");
    printf("4. print_list_reverse() - вивід у зворотному порядку\n");
    printf("5. Демонстрація різних способів обходу\n\n");

    printf("Особливості односв'язного списку:\n");
    printf("- Обхід тільки в одному напрямку\n");
    printf("- Довжина обчислюється за O(n)\n");
    printf("- Легке додавання на початок O(1)\n");
    printf("- Пошук за O(n)\n");
    printf("- Видалення вимагає знання попереднього елемента\n");

    return 0;
}