#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
ЗАВДАННЯ 10: Рекурсивна та ітераційна версії функції reverse_list

Реалізуємо дві версії функції для зміни порядку елементів списку
на протилежний без створення нових елементів.
*/

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;
};

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

// Додавання на початок
Nameval* addfront(Nameval *listp, Nameval *newp) {
    newp->next = listp;
    return newp;
}

// Функція для виведення списку
void print_list(Nameval *listp, const char *title) {
    printf("\n=== %s ===\n", title);
    if (listp == NULL) {
        printf("Список порожній\n");
        return;
    }

    printf("Адреси елементів для відстеження: ");
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("%p ", (void*)p);
        if (p->next != NULL) printf("-> ");
    }
    printf("\n");

    printf("Елементи: ");
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("%s(%d)", p->name, p->value);
        if (p->next != NULL) printf(" -> ");
    }
    printf("\n");
}

// ============= ІТЕРАЦІЙНА ВЕРСІЯ =============

Nameval* reverse_list_iterative(Nameval *listp) {
    if (listp == NULL || listp->next == NULL) {
        return listp; // Порожній список або один елемент
    }

    Nameval *prev = NULL;
    Nameval *current = listp;
    Nameval *next = NULL;

    printf("\nІтераційний процес розвороту:\n");
    int step = 1;

    while (current != NULL) {
        printf("Крок %d:\n", step++);
        printf("  prev=%p, current=%p (%s), next=?\n",
               (void*)prev, (void*)current, current->name);

        // Зберігаємо наступний елемент
        next = current->next;
        printf("  Зберігаємо next=%p\n", (void*)next);

        // Розвертаємо вказівник
        current->next = prev;
        printf("  Встановлюємо current->next=%p\n", (void*)prev);

        // Переміщуємо вказівники
        prev = current;
        current = next;
        printf("  Переміщуємо: prev=%p, current=%p\n",
               (void*)prev, (void*)current);
        printf("\n");
    }

    printf("Новий початок списку: %p\n", (void*)prev);
    return prev; // prev тепер вказує на новий початок
}

// ============= РЕКУРСИВНА ВЕРСІЯ =============

Nameval* reverse_list_recursive(Nameval *listp) {
    // Базовий випадок: порожній список або один елемент
    if (listp == NULL || listp->next == NULL) {
        if (listp != NULL) {
            printf("Базовий випадок досягнуто: %s (адреса %p)\n",
                   listp->name, (void*)listp);
        }
        return listp;
    }

    printf("Рекурсивний виклик для: %s (адреса %p)\n",
           listp->name, (void*)listp);
    printf("  Переходимо до наступного елемента: %s\n", listp->next->name);

    // Рекурсивно розвертаємо решту списку
    Nameval *new_head = reverse_list_recursive(listp->next);

    printf("Повертаємося з рекурсії до: %s\n", listp->name);
    printf("  new_head=%p (%s)\n", (void*)new_head,
           new_head ? new_head->name : "NULL");

    // Розвертаємо поточний зв'язок
    printf("  Було: %s->next = %s\n", listp->name,
           listp->next ? listp->next->name : "NULL");

    listp->next->next = listp;  // Розвертаємо зв'язок
    listp->next = NULL;         // Обриваємо старий зв'язок

    printf("  Стало: %s->next = NULL, попередній->next = %s\n",
           listp->name, listp->name);

    return new_head;
}

// ============= ДОПОМІЖНІ ФУНКЦІЇ =============

// Створення копії списку для тестування
Nameval* copy_list(Nameval *listp) {
    if (listp == NULL) return NULL;

    Nameval *new_list = NULL;
    Nameval *last = NULL;

    for (Nameval *p = listp; p != NULL; p = p->next) {
        Nameval *new_node = newitem(p->name, p->value);
        if (new_node == NULL) return new_list;

        if (new_list == NULL) {
            new_list = last = new_node;
        } else {
            last->next = new_node;
            last = new_node;
        }
    }

    return new_list;
}

// Перевірка цілісності списку
int verify_list_integrity(Nameval *listp, int expected_length) {
    int count = 0;
    Nameval *slow = listp;
    Nameval *fast = listp;

    // Перевірка на цикли (алгоритм "заєць та черепаха")
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        count += 2;

        if (slow == fast && count < expected_length * 2) {
            printf("ПОМИЛКА: Виявлено цикл у списку!\n");
            return 0;
        }
    }

    // Підрахунок точної довжини
    count = 0;
    for (Nameval *p = listp; p != NULL; p = p->next) {
        count++;
        if (count > expected_length + 5) { // Запобіжник
            printf("ПОМИЛКА: Список занадто довгий або має цикл!\n");
            return 0;
        }
    }

    if (count != expected_length) {
        printf("ПОПЕРЕДЖЕННЯ: Очікувалося %d елементів, знайдено %d\n",
               expected_length, count);
        return 0;
    }

    printf("Цілісність списку перевірено: %d елементів, циклів немає\n", count);
    return 1;
}

// Звільнення пам'яті
void free_list(Nameval *listp) {
    Nameval *next;
    for (Nameval *p = listp; p != NULL; p = next) {
        next = p->next;
        free(p->name);
        free(p);
    }
}

// ============= ДЕМОНСТРАЦІЇ =============

void demonstrate_iterative() {
    printf("=== ДЕМОНСТРАЦІЯ ІТЕРАЦІЙНОЇ ВЕРСІЇ ===\n");

    // Створюємо тестовий список
    Nameval *list = NULL;
    list = addfront(list, newitem("David", 40));
    list = addfront(list, newitem("Charlie", 30));
    list = addfront(list, newitem("Bob", 20));
    list = addfront(list, newitem("Alice", 10));

    print_list(list, "Оригінальний список");

    // Розвертаємо ітераційно
    printf("\nПочинаємо ітераційний розворот:\n");
    list = reverse_list_iterative(list);

    print_list(list, "Розвернутий список (ітераційно)");

    // Перевіряємо цілісність
    verify_list_integrity(list, 4);

    free_list(list);
}

void demonstrate_recursive() {
    printf("\n=== ДЕМОНСТРАЦІЯ РЕКУРСИВНОЇ ВЕРСІЇ ===\n");

    // Створюємо тестовий список
    Nameval *list = NULL;
    list = addfront(list, newitem("David", 40));
    list = addfront(list, newitem("Charlie", 30));
    list = addfront(list, newitem("Bob", 20));
    list = addfront(list, newitem("Alice", 10));

    print_list(list, "Оригінальний список");

    // Розвертаємо рекурсивно
    printf("\nПочинаємо рекурсивний розворот:\n");
    list = reverse_list_recursive(list);

    print_list(list, "Розвернутий список (рекурсивно)");

    // Перевіряємо цілісність
    verify_list_integrity(list, 4);

    free_list(list);
}

void compare_versions() {
    printf("\n=== ПОРІВНЯННЯ ВЕРСІЙ ===\n");

    printf("ІТЕРАЦІЙНА ВЕРСІЯ:\n");
    printf("+ Складність часу: O(n)\n");
    printf("+ Складність пам'яті: O(1) - константна\n");
    printf("+ Не використовує стек викликів\n");
    printf("+ Безпечна для великих списків\n");
    printf("+ Легша для розуміння покроково\n");
    printf("- Більше змінних для відстеження стану\n\n");

    printf("РЕКУРСИВНА ВЕРСІЯ:\n");
    printf("+ Складність часу: O(n)\n");
    printf("+ Складність пам'яті: O(n) - через стек рекурсії\n");
    printf("+ Більш елегантний код\n");
    printf("+ Природна для рекурсивних структур\n");
    printf("- Може призвести до stack overflow для великих списків\n");
    printf("- Важча для покрокового відстеження\n\n");

    printf("РЕКОМЕНДАЦІЇ:\n");
    printf("• Для великих списків (>1000 елементів): ітераційна\n");
    printf("• Для навчальних цілей: рекурсивна\n");
    printf("• Для продакшн коду: ітераційна (безпечніша)\n");
}

void test_edge_cases() {
    printf("\n=== ТЕСТУВАННЯ КРАЙНІХ ВИПАДКІВ ===\n");

    // Тест 1: Порожній список
    printf("\n1. Порожній список:\n");
    Nameval *empty = NULL;
    empty = reverse_list_iterative(empty);
    print_list(empty, "Порожній після розвороту");

    // Тест 2: Один елемент
    printf("\n2. Список з одним елементом:\n");
    Nameval *single = newitem("OnlyOne", 42);
    print_list(single, "До розвороту");
    single = reverse_list_iterative(single);
    print_list(single, "Після розвороту");
    free_list(single);

    // Тест 3: Два елементи
    printf("\n3. Список з двома елементами:\n");
    Nameval *two = NULL;
    two = addfront(two, newitem("Second", 2));
    two = addfront(two, newitem("First", 1));
    print_list(two, "До розвороту");
    two = reverse_list_iterative(two);
    print_list(two, "Після розвороту");
    free_list(two);
}

void demonstrate_double_reverse() {
    printf("\n=== ПОДВІЙНИЙ РОЗВОРОТ ===\n");

    // Створюємо список
    Nameval *list = NULL;
    list = addfront(list, newitem("Third", 3));
    list = addfront(list, newitem("Second", 2));
    list = addfront(list, newitem("First", 1));

    print_list(list, "Оригінальний список");

    // Перший розворот
    list = reverse_list_iterative(list);
    print_list(list, "Після першого розвороту");

    // Другий розворот
    list = reverse_list_iterative(list);
    print_list(list, "Після другого розвороту");

    printf("Результат: список повернувся до оригінального стану!\n");
    printf("Це підтверджує правильність алгоритму.\n");

    free_list(list);
}

int main(void) {
    printf("=== Завдання 10: Розворот списку ===\n");

    demonstrate_iterative();
    demonstrate_recursive();
    compare_versions();
    test_edge_cases();
    demonstrate_double_reverse();

    printf("\n=== АЛГОРИТМІЧНИЙ АНАЛІЗ ===\n");
    printf("Обидві версії:\n");
    printf("• Час виконання: O(n) - кожен елемент обробляється один раз\n");
    printf("• Простір: ітераційна O(1), рекурсивна O(n)\n");
    printf("• Стабільність: обидві стабільні (не змінюють відносний порядок однакових елементів)\n");
    printf("• Місце: in-place (не створюють нових елементів)\n\n");

    printf("Ключові моменти реалізації:\n");
    printf("1. Ітераційна: три вказівники (prev, current, next)\n");
    printf("2. Рекурсивна: базовий випадок + розворот зв'язків при поверненні\n");
    printf("3. Важливо: обнулити next у останнього елемента\n");
    printf("4. Повертати новий початок списку\n\n");

    printf("Практичне застосування:\n");
    printf("• Undo/Redo операції\n");
    printf("• Алгоритми сортування\n");
    printf("• Обробка стеків та черг\n");
    printf("• Інвертування послідовностей\n");

    return 0;
}