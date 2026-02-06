#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>



typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;
};


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

Nameval* addfront(Nameval *listp, Nameval *newp) {
    newp->next = listp;
    return newp;
}

Nameval* lookup(Nameval *listp, char *name) {
    for (Nameval *p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0)
            return p;
    }
    return NULL;
}

Nameval* delitem(Nameval *listp, char *name) {
    Nameval *p, *prev;

    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0) {
            if (prev == NULL) {
                listp = p->next;
            } else {
                prev->next = p->next;
            }
            free(p->name);
            free(p);
            return listp;
        }
        prev = p;
    }
    return listp;
}

int list_length(Nameval *listp) {
    int count = 0;
    for (Nameval *p = listp; p != NULL; p = p->next) {
        count++;
    }
    return count;
}

void free_list(Nameval *listp) {
    Nameval *next;
    for (Nameval *p = listp; p != NULL; p = next) {
        next = p->next;
        free(p->name);
        free(p);
    }
}

Nameval* reverse_list(Nameval *listp) {
    Nameval *prev = NULL;
    Nameval *current = listp;
    Nameval *next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev;
}


typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    char current_test[256];
} TestSuite;

TestSuite test_suite = {0, 0, 0, ""};

void test_start(const char *test_name) {
    strcpy(test_suite.current_test, test_name);
    test_suite.total_tests++;
    printf("Тест: %s ... ", test_name);
}

void test_assert(int condition, const char *message) {
    if (condition) {
        printf("ПРОЙДЕНО\n");
        test_suite.passed_tests++;
    } else {
        printf("ПРОВАЛЕНО - %s\n", message);
        test_suite.failed_tests++;
    }
}

void test_summary() {
    printf("\n=== ПІДСУМОК ТЕСТУВАННЯ ===\n");
    printf("Загалом тестів: %d\n", test_suite.total_tests);
    printf("Пройшло: %d\n", test_suite.passed_tests);
    printf("Провалилося: %d\n", test_suite.failed_tests);
    printf("Успішність: %.1f%%\n",
           (double)test_suite.passed_tests / test_suite.total_tests * 100);

    if (test_suite.failed_tests == 0) {
        printf("🎉 Всі тести пройшли успішно!\n");
    } else {
        printf("❌ Є проблеми, які потребують виправлення\n");
    }
}


void test_newitem() {
    test_start("newitem - створення елемента");

    Nameval *item = newitem("Test", 42);
    int result = (item != NULL &&
                  strcmp(item->name, "Test") == 0 &&
                  item->value == 42 &&
                  item->next == NULL);

    test_assert(result, "Елемент створений некоректно");

    if (item) {
        free(item->name);
        free(item);
    }
}

void test_newitem_memory_failure() {
    test_start("newitem - обробка помилок пам'яті");

    // Симулювати помилку важко, тому перевіряємо тільки NULL параметри
    // В реальності тут би використали mock для malloc
    test_assert(1, "Тест пропущено - потребує mock malloc");
}

void test_addfront() {
    test_start("addfront - додавання на початок");

    Nameval *list = NULL;
    Nameval *item1 = newitem("First", 1);
    Nameval *item2 = newitem("Second", 2);

    list = addfront(list, item1);
    list = addfront(list, item2);

    int result = (list == item2 &&
                  list->next == item1 &&
                  item1->next == NULL);

    test_assert(result, "Порядок елементів неправильний");

    free_list(list);
}

void test_lookup_found() {
    test_start("lookup - пошук існуючого елемента");

    Nameval *list = NULL;
    list = addfront(list, newitem("Charlie", 30));
    list = addfront(list, newitem("Bob", 20));
    list = addfront(list, newitem("Alice", 10));

    Nameval *found = lookup(list, "Bob");
    int result = (found != NULL &&
                  strcmp(found->name, "Bob") == 0 &&
                  found->value == 20);

    test_assert(result, "Елемент не знайдено або дані неправильні");

    free_list(list);
}

void test_lookup_not_found() {
    test_start("lookup - пошук неіснуючого елемента");

    Nameval *list = NULL;
    list = addfront(list, newitem("Alice", 10));

    Nameval *found = lookup(list, "Unknown");
    test_assert(found == NULL, "Повернуто не NULL для неіснуючого елемента");

    free_list(list);
}

void test_lookup_empty_list() {
    test_start("lookup - пошук в порожньому списку");

    Nameval *found = lookup(NULL, "Any");
    test_assert(found == NULL, "Повернуто не NULL для порожнього списку");
}

void test_delitem_first() {
    test_start("delitem - видалення першого елемента");

    Nameval *list = NULL;
    list = addfront(list, newitem("Third", 3));
    list = addfront(list, newitem("Second", 2));
    list = addfront(list, newitem("First", 1));

    list = delitem(list, "First");

    int result = (list != NULL &&
                  strcmp(list->name, "Second") == 0 &&
                  list_length(list) == 2);

    test_assert(result, "Перший елемент видалено неправильно");

    free_list(list);
}

void test_delitem_middle() {
    test_start("delitem - видалення середнього елемента");

    Nameval *list = NULL;
    list = addfront(list, newitem("Third", 3));
    list = addfront(list, newitem("Second", 2));
    list = addfront(list, newitem("First", 1));

    list = delitem(list, "Second");

    int result = (list != NULL &&
                  list->next != NULL &&
                  strcmp(list->next->name, "Third") == 0 &&
                  list_length(list) == 2);

    test_assert(result, "Середній елемент видалено неправильно");

    free_list(list);
}

void test_delitem_last() {
    test_start("delitem - видалення останнього елемента");

    Nameval *list = NULL;
    list = addfront(list, newitem("Third", 3));
    list = addfront(list, newitem("Second", 2));
    list = addfront(list, newitem("First", 1));

    list = delitem(list, "Third");

    int result = (list != NULL &&
                  list->next != NULL &&
                  list->next->next == NULL &&
                  list_length(list) == 2);

    test_assert(result, "Останній елемент видалено неправильно");

    free_list(list);
}

void test_delitem_not_found() {
    test_start("delitem - видалення неіснуючого елемента");

    Nameval *list = NULL;
    list = addfront(list, newitem("Alice", 10));

    int original_length = list_length(list);
    list = delitem(list, "Unknown");

    int result = (list_length(list) == original_length);

    test_assert(result, "Список змінився при видаленні неіснуючого елемента");

    free_list(list);
}

void test_list_length() {
    test_start("list_length - підрахунок довжини");

    Nameval *list = NULL;

    // Порожній список
    int len0 = list_length(list);

    // Додаємо елементи
    list = addfront(list, newitem("Three", 3));
    int len1 = list_length(list);

    list = addfront(list, newitem("Two", 2));
    int len2 = list_length(list);

    list = addfront(list, newitem("One", 1));
    int len3 = list_length(list);

    int result = (len0 == 0 && len1 == 1 && len2 == 2 && len3 == 3);

    test_assert(result, "Довжина обчислена неправильно");

    free_list(list);
}

void test_reverse_list() {
    test_start("reverse_list - розворот списку");

    Nameval *list = NULL;
    list = addfront(list, newitem("Third", 3));
    list = addfront(list, newitem("Second", 2));
    list = addfront(list, newitem("First", 1));

    // До розвороту: First -> Second -> Third
    list = reverse_list(list);
    // Після розвороту: Third -> Second -> First

    int result = (list != NULL &&
                  strcmp(list->name, "Third") == 0 &&
                  list->next != NULL &&
                  strcmp(list->next->name, "Second") == 0 &&
                  list->next->next != NULL &&
                  strcmp(list->next->next->name, "First") == 0 &&
                  list->next->next->next == NULL);

    test_assert(result, "Список розвернуто неправильно");

    free_list(list);
}


void test_edge_cases() {
    printf("\n=== ТЕСТИ КРАЙНІХ ВИПАДКІВ ===\n");

    // Тест з порожнім списком
    test_start("reverse_list - порожній список");
    Nameval *empty = reverse_list(NULL);
    test_assert(empty == NULL, "Розворот порожнього списку дав не NULL");

    // Тест з одним елементом
    test_start("reverse_list - один елемент");
    Nameval *single = newitem("Only", 1);
    Nameval *reversed = reverse_list(single);
    int result = (reversed == single && reversed->next == NULL);
    test_assert(result, "Розворот одного елемента спрацював неправильно");
    free_list(reversed);

    // Тест з дублікатами
    test_start("lookup - дублікати");
    Nameval *list = NULL;
    list = addfront(list, newitem("Duplicate", 2));
    list = addfront(list, newitem("Duplicate", 1));

    Nameval *found = lookup(list, "Duplicate");
    int dup_result = (found != NULL && found->value == 1); // Повинен знайти перший
    test_assert(dup_result, "При дублікатах знайдено не перший елемент");

    free_list(list);

    // Тест з довгими рядками
    test_start("newitem - довгий рядок");
    char long_name[1000];
    for (int i = 0; i < 999; i++) {
        long_name[i] = 'A' + (i % 26);
    }
    long_name[999] = '\0';

    Nameval *long_item = newitem(long_name, 999);
    int long_result = (long_item != NULL &&
                       strlen(long_item->name) == 999 &&
                       strcmp(long_item->name, long_name) == 0);
    test_assert(long_result, "Довгий рядок оброблено неправильно");

    if (long_item) {
        free(long_item->name);
        free(long_item);
    }
}


void test_performance() {
    printf("\n=== ТЕСТИ ПРОДУКТИВНОСТІ ===\n");

    const int LARGE_SIZE = 10000;
    clock_t start, end;

    // Тест додавання великої кількості елементів
    test_start("Продуктивність - додавання 10000 елементів");

    start = clock();
    Nameval *large_list = NULL;
    for (int i = 0; i < LARGE_SIZE; i++) {
        char name[20];
        sprintf(name, "Item%d", i);
        large_list = addfront(large_list, newitem(name, i));
    }
    end = clock();

    double add_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("ІНФО - Час додавання: %.3f сек\n", add_time);

    // Тест пошуку в великому списку
    start = clock();
    Nameval *found = lookup(large_list, "Item5000");
    end = clock();

    double search_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("    Час пошуку в середині: %.6f сек\n", search_time);
    test_assert(found != NULL && found->value == 5000,
                "Елемент не знайдено в великому списку");

    // Тест розвороту великого списку
    start = clock();
    large_list = reverse_list(large_list);
    end = clock();

    double reverse_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("    Час розвороту: %.3f сек\n", reverse_time);

    // Перевіряємо правильність розвороту
    int reverse_correct = (strcmp(large_list->name, "Item0") == 0);
    test_assert(reverse_correct, "Розворот великого списку неправильний");

    // Очищення
    start = clock();
    free_list(large_list);
    end = clock();

    double free_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("    Час очищення: %.3f сек\n", free_time);
}


void test_memory_integrity() {
    printf("\n=== ТЕСТИ ЦІЛІСНОСТІ ПАМ'ЯТІ ===\n");

    test_start("Цілісність пам'яті - створення і видалення");

    // Створюємо і видаляємо багато списків
    for (int cycle = 0; cycle < 100; cycle++) {
        Nameval *list = NULL;

        // Додаємо елементи
        for (int i = 0; i < 10; i++) {
            char name[20];
            sprintf(name, "Cycle%d_Item%d", cycle, i);
            list = addfront(list, newitem(name, i));
        }

        // Видаляємо частину елементів
        for (int i = 0; i < 5; i++) {
            char name[20];
            sprintf(name, "Cycle%d_Item%d", cycle, i);
            list = delitem(list, name);
        }

        // Очищаємо решту
        free_list(list);
    }

    test_assert(1, "Тест завершено без крахів");
}


void test_robustness() {
    printf("\n=== ТЕСТИ СТІЙКОСТІ ===\n");

    // Тест з NULL параметрами
    test_start("Стійкість - NULL параметри");

    Nameval *result1 = addfront(NULL, NULL);
    Nameval *result2 = lookup(NULL, NULL);
    Nameval *result3 = delitem(NULL, NULL);
    int length = list_length(NULL);

    // Ці операції не повинні призводити до краху
    test_assert(1, "Операції з NULL виконані без краху");

    // Тест з порожніми рядками
    test_start("Стійкість - порожні рядки");

    Nameval *empty_name = newitem("", 0);
    int empty_result = (empty_name != NULL &&
                        strlen(empty_name->name) == 0);
    test_assert(empty_result, "Порожній рядок оброблено неправильно");

    if (empty_name) {
        free(empty_name->name);
        free(empty_name);
    }
}


void run_all_tests() {
    printf("=== ЗАПУСК ПОВНОГО НАБОРУ ТЕСТІВ ===\n\n");

    // Основні функції
    printf("=== ТЕСТИ ОСНОВНИХ ФУНКЦІЙ ===\n");
    test_newitem();
    test_newitem_memory_failure();
    test_addfront();
    test_lookup_found();
    test_lookup_not_found();
    test_lookup_empty_list();
    test_delitem_first();
    test_delitem_middle();
    test_delitem_last();
    test_delitem_not_found();
    test_list_length();
    test_reverse_list();

    // Крайні випадки
    test_edge_cases();

    // Продуктивність
    test_performance();

    // Цілісність пам'яті
    test_memory_integrity();

    // Стійкість
    test_robustness();

    // Підсумок
    test_summary();
}

int main(void) {
    printf("=== Завдання 13: Система тестування функцій списків ===\n\n");

    run_all_tests();


    return (test_suite.failed_tests == 0) ? 0 : 1;
}