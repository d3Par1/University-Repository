#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

// Додавання на початок (з лекції)
Nameval* addfront(Nameval *listp, Nameval *newp) {
    newp->next = listp;
    return newp;
}

// Пошук елемента (з лекції)
Nameval* lookup(Nameval *listp, char *name) {
    for (Nameval *p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0)
            return p;
    }
    return NULL;
}

// Функція для виведення списку
void print_list(Nameval *listp, const char *title) {
    printf("\n=== %s ===\n", title);
    if (listp == NULL) {
        printf("Список порожній\n");
        return;
    }

    int count = 0;
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("  [%d] %s = %d\n", count++, p->name, p->value);
    }
    printf("Загальна кількість: %d\n", count);
}


// 1. КОПІЮВАННЯ СПИСКУ
Nameval* copy_list(Nameval *listp) {
    if (listp == NULL) return NULL;

    // Створюємо новий список
    Nameval *new_list = NULL;
    Nameval *last = NULL;

    // Проходимо оригінальний список
    for (Nameval *p = listp; p != NULL; p = p->next) {
        Nameval *new_node = newitem(p->name, p->value);
        if (new_node == NULL) {
            // Якщо помилка - звільняємо вже створену частину
            // (функцію звільнення реалізуємо нижче)
            return new_list;
        }

        if (new_list == NULL) {
            // Перший елемент
            new_list = last = new_node;
        } else {
            // Додаємо в кінець для збереження порядку
            last->next = new_node;
            last = new_node;
        }
    }

    return new_list;
}

// 2. ЗЛИТТЯ ДВОХ СПИСКІВ
Nameval* merge_lists(Nameval *list1, Nameval *list2) {
    if (list1 == NULL) return list2;
    if (list2 == NULL) return list1;

    // Знаходимо кінець першого списку
    Nameval *last = list1;
    while (last->next != NULL) {
        last = last->next;
    }

    // Приєднуємо другий список
    last->next = list2;

    return list1;
}

// 3. РОЗБИТТЯ СПИСКУ НА ДВА (за індексом)
typedef struct {
    Nameval *first_part;
    Nameval *second_part;
} SplitResult;

SplitResult split_list(Nameval *listp, int split_index) {
    SplitResult result = {NULL, NULL};

    if (listp == NULL || split_index <= 0) {
        result.second_part = listp;
        return result;
    }

    // Знаходимо елемент перед точкою розбиття
    Nameval *current = listp;
    for (int i = 0; i < split_index - 1 && current->next != NULL; i++) {
        current = current->next;
    }

    // Розбиваємо список
    result.first_part = listp;
    result.second_part = current->next;
    current->next = NULL;

    return result;
}

// 4. ВСТАВКА ПЕРЕД КОНКРЕТНИМ ЕЛЕМЕНТОМ
Nameval* insert_before(Nameval *listp, char *target_name, Nameval *new_node) {
    if (listp == NULL || new_node == NULL) return listp;

    // Якщо вставляємо перед першим елементом
    if (strcmp(listp->name, target_name) == 0) {
        new_node->next = listp;
        return new_node;
    }

    // Шукаємо елемент перед цільовим
    for (Nameval *p = listp; p->next != NULL; p = p->next) {
        if (strcmp(p->next->name, target_name) == 0) {
            new_node->next = p->next;
            p->next = new_node;
            return listp;
        }
    }

    printf("Елемент '%s' не знайдено для вставки перед ним\n", target_name);
    return listp;
}

// 5. ВСТАВКА ПІСЛЯ КОНКРЕТНОГО ЕЛЕМЕНТУ
Nameval* insert_after(Nameval *listp, char *target_name, Nameval *new_node) {
    if (listp == NULL || new_node == NULL) return listp;

    // Шукаємо цільовий елемент
    for (Nameval *p = listp; p != NULL; p = p->next) {
        if (strcmp(p->name, target_name) == 0) {
            new_node->next = p->next;
            p->next = new_node;
            return listp;
        }
    }

    printf("Елемент '%s' не знайдено для вставки після нього\n", target_name);
    return listp;
}

// Функція для звільнення пам'яті списку
void free_list(Nameval *listp) {
    Nameval *next;
    for (Nameval *p = listp; p != NULL; p = next) {
        next = p->next;
        free(p->name);
        free(p);
    }
}


void demonstrate_copying() {
    printf("\n=== ДЕМОНСТРАЦІЯ КОПІЮВАННЯ ===\n");

    // Створюємо оригінальний список
    Nameval *original = NULL;
    original = addfront(original, newitem("Charlie", 30));
    original = addfront(original, newitem("Bob", 20));
    original = addfront(original, newitem("Alice", 10));

    print_list(original, "Оригінальний список");

    // Копіюємо
    Nameval *copy = copy_list(original);
    print_list(copy, "Скопійований список");

    // Перевіряємо, що це справді копія (різні адреси)
    printf("\nПеревірка незалежності копії:\n");
    printf("Оригінал: Alice на адресі %p\n", (void*)original);
    printf("Копія:    Alice на адресі %p\n", (void*)copy);
    printf("Це різні об'єкти: %s\n", (original != copy) ? "ТАК" : "НІ");

    // Модифікуємо оригінал
    original->value = 999;
    printf("\nПісля зміни значення Alice в оригіналі на 999:\n");
    printf("Оригінал: Alice = %d\n", original->value);
    printf("Копія:    Alice = %d\n", copy->value);

    free_list(original);
    free_list(copy);
}

void demonstrate_merging() {
    printf("\n=== ДЕМОНСТРАЦІЯ ЗЛИТТЯ ===\n");

    // Створюємо два списки
    Nameval *list1 = NULL;
    list1 = addfront(list1, newitem("Bob", 20));
    list1 = addfront(list1, newitem("Alice", 10));

    Nameval *list2 = NULL;
    list2 = addfront(list2, newitem("David", 40));
    list2 = addfront(list2, newitem("Charlie", 30));

    print_list(list1, "Перший список");
    print_list(list2, "Другий список");

    // Зливаємо
    Nameval *merged = merge_lists(list1, list2);
    print_list(merged, "Об'єднаний список");

    printf("\nУВАГА: Після злиття list2 стає частиною merged.\n");
    printf("Не треба окремо звільняти list2!\n");

    free_list(merged);
}

void demonstrate_splitting() {
    printf("\n=== ДЕМОНСТРАЦІЯ РОЗБИТТЯ ===\n");

    // Створюємо список з 5 елементів
    Nameval *original = NULL;
    original = addfront(original, newitem("Eve", 50));
    original = addfront(original, newitem("David", 40));
    original = addfront(original, newitem("Charlie", 30));
    original = addfront(original, newitem("Bob", 20));
    original = addfront(original, newitem("Alice", 10));

    print_list(original, "Оригінальний список (5 елементів)");

    // Розбиваємо після 2-го елемента
    printf("\nРозбиваємо після 2-го елемента:\n");
    SplitResult result = split_list(original, 2);

    print_list(result.first_part, "Перша частина");
    print_list(result.second_part, "Друга частина");

    // Звільняємо обидві частини
    free_list(result.first_part);
    free_list(result.second_part);
}

void demonstrate_insertions() {
    printf("\n=== ДЕМОНСТРАЦІЯ ВСТАВОК ===\n");

    // Створюємо базовий список
    Nameval *list = NULL;
    list = addfront(list, newitem("Charlie", 30));
    list = addfront(list, newitem("Alice", 10));

    print_list(list, "Початковий список");

    // Вставка ПІСЛЯ Alice
    printf("\nВставляємо 'Bob' після 'Alice':\n");
    list = insert_after(list, "Alice", newitem("Bob", 20));
    print_list(list, "Після вставки Bob");

    // Вставка ПЕРЕД Charlie
    printf("\nВставляємо 'BobHalf' перед 'Charlie':\n");
    list = insert_before(list, "Charlie", newitem("BobHalf", 25));
    print_list(list, "Після вставки BobHalf");

    // Вставка перед першим елементом
    printf("\nВставляємо 'Zero' перед 'Alice' (перший елемент):\n");
    list = insert_before(list, "Alice", newitem("Zero", 0));
    print_list(list, "Після вставки Zero на початок");

    // Спроба вставки перед неіснуючим елементом
    printf("\nСпроба вставки перед неіснуючим 'Unknown':\n");
    list = insert_before(list, "Unknown", newitem("Never", 999));
    print_list(list, "Список не змінився");

    free_list(list);
}

void compare_insertion_complexity() {
    printf("\n=== ПОРІВНЯННЯ СКЛАДНОСТІ ВСТАВОК ===\n");

    printf("ВСТАВКА ПІСЛЯ елемента:\n");
    printf("+ Складність: O(n) - потрібно знайти елемент\n");
    printf("+ Після знаходження: O(1) операція\n");
    printf("+ Простіша реалізація\n");
    printf("+ Не змінює голову списку\n\n");

    printf("ВСТАВКА ПЕРЕД елементом:\n");
    printf("+ Складність: O(n) - потрібно знайти попередній елемент\n");
    printf("+ Особливий випадок: вставка перед першим елементом\n");
    printf("+ Може змінювати голову списку\n");
    printf("+ Складніша реалізація через особливий випадок\n\n");

    printf("ВИСНОВОК:\n");
    printf("Вставка ПІСЛЯ простіша в реалізації, але обидві мають O(n) складність.\n");
    printf("Різниця тільки в тому, що вставка ПЕРЕД може змінювати голову списку.\n");
}

void demonstrate_reusability() {
    printf("\n=== ВИКОРИСТАННЯ ІСНУЮЧИХ ФУНКЦІЙ ===\n");

    printf("Які функції з лекції можна використати:\n");
    printf("✓ newitem() - для створення нових елементів\n");
    printf("✓ addfront() - як основа для вставки перед першим\n");
    printf("✓ lookup() - для пошуку цільових елементів\n\n");

    printf("Що довелося написати заново:\n");
    printf("• copy_list() - потрібен спеціальний алгоритм збереження порядку\n");
    printf("• merge_lists() - специфічна логіка з'єднання\n");
    printf("• split_list() - розрив зв'язків у певній точці\n");
    printf("• insert_before/after() - специфічна логіка вставки\n\n");

    printf("ПОВТОРНЕ ВИКОРИСТАННЯ: ~40%\n");
    printf("Базові операції (newitem, lookup) використовуються активно,\n");
    printf("але складні операції потребують нової реалізації.\n");
}

int main(void) {
    printf("=== Завдання 9: Додаткові операції зі списком ===\n");

    demonstrate_copying();
    demonstrate_merging();
    demonstrate_splitting();
    demonstrate_insertions();
    compare_insertion_complexity();
    demonstrate_reusability();


    return 0;
}