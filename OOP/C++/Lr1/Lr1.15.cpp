#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct Treenode Treenode;

struct Treenode {
    char *word;
    int count;
    Treenode *left;
    Treenode *right;
};


Treenode* newtree(char *word, int count) {
    Treenode *newp = (Treenode *) malloc(sizeof(Treenode));
    if (newp == NULL) return NULL;

    newp->word = (char *) malloc(strlen(word) + 1);
    if (newp->word == NULL) {
        free(newp);
        return NULL;
    }

    strcpy(newp->word, word);
    newp->count = count;
    newp->left = newp->right = NULL;
    return newp;
}

Treenode* insert(Treenode *treep, char *word, int count) {
    int cmp;

    if (treep == NULL) {
        return newtree(word, count);
    }

    cmp = strcmp(word, treep->word);
    if (cmp == 0) {
        treep->count += count;
    } else if (cmp < 0) {
        treep->left = insert(treep->left, word, count);
    } else {
        treep->right = insert(treep->right, word, count);
    }

    return treep;
}

void freetree(Treenode *treep) {
    if (treep == NULL) return;
    freetree(treep->left);
    freetree(treep->right);
    free(treep->word);
    free(treep);
}


int recursive_calls = 0;  // Лічильник рекурсивних викликів

Treenode* lookup_recursive(Treenode *treep, char *word) {
    int cmp;

    recursive_calls++;  // Підраховуємо виклики

    if (treep == NULL)
        return NULL;

    cmp = strcmp(word, treep->word);
    if (cmp == 0)
        return treep;
    else if (cmp < 0)
        return lookup_recursive(treep->left, word);
    else
        return lookup_recursive(treep->right, word);
}


int iterative_comparisons = 0;  // Лічильник порівнянь

Treenode* lookup_iterative(Treenode *treep, char *word) {
    int cmp;
    Treenode *current = treep;

    iterative_comparisons = 0;  // Скидаємо лічильник

    while (current != NULL) {
        iterative_comparisons++;  // Підраховуємо порівняння

        cmp = strcmp(word, current->word);
        if (cmp == 0)
            return current;
        else if (cmp < 0)
            current = current->left;
        else
            current = current->right;
    }

    return NULL;
}


typedef struct {
    double recursive_time;
    double iterative_time;
    int recursive_calls_count;
    int iterative_comparisons_count;
    int tree_height;
    int found;
} PerformanceResult;

PerformanceResult measure_performance(Treenode *tree, char *search_word) {
    PerformanceResult result = {0};
    clock_t start, end;
    Treenode *found;

    // Тестування рекурсивної версії
    recursive_calls = 0;
    start = clock();
    found = lookup_recursive(tree, search_word);
    end = clock();

    result.recursive_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    result.recursive_calls_count = recursive_calls;
    result.found = (found != NULL);

    // Тестування ітераційної версії
    start = clock();
    found = lookup_iterative(tree, search_word);
    end = clock();

    result.iterative_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    result.iterative_comparisons_count = iterative_comparisons;

    return result;
}

// Обчислення висоти дерева
int tree_height(Treenode *treep) {
    if (treep == NULL) return 0;

    int left_height = tree_height(treep->left);
    int right_height = tree_height(treep->right);

    return 1 + (left_height > right_height ? left_height : right_height);
}


// Створення збалансованого дерева
Treenode* create_balanced_tree(char words[][20], int start, int end) {
    if (start > end) return NULL;

    int mid = (start + end) / 2;
    Treenode *root = newtree(words[mid], 1);

    if (root != NULL) {
        root->left = create_balanced_tree(words, start, mid - 1);
        root->right = create_balanced_tree(words, mid + 1, end);
    }

    return root;
}

// Створення незбалансованого дерева (лінійного)
Treenode* create_linear_tree(char words[][20], int count) {
    Treenode *root = NULL;

    // Вставляємо в порядку зростання (створює лінійне дерево)
    for (int i = 0; i < count; i++) {
        root = insert(root, words[i], 1);
    }

    return root;
}


void demonstrate_basic_comparison() {
    printf("=== БАЗОВЕ ПОРІВНЯННЯ LOOKUP vs NVLOOKUP ===\n");

    // Створюємо тестове дерево
    char test_words[][20] = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon", "mango"
    };
    int word_count = sizeof(test_words) / sizeof(test_words[0]);

    printf("\nСтворюємо збалансоване дерево з %d слів...\n", word_count);
    Treenode *balanced_tree = create_balanced_tree(test_words, 0, word_count - 1);

    printf("Висота дерева: %d\n", tree_height(balanced_tree));

    // Тестуємо пошук різних елементів
    char search_words[][20] = {"apple", "grape", "mango", "cherry", "unknown"};
    int search_count = sizeof(search_words) / sizeof(search_words[0]);

    printf("\n%-12s %-8s %-12s %-8s %-12s %-10s\n",
           "Слово", "Знайдено", "Рек.час(μs)", "Рек.викл", "Ітер.час(μs)", "Ітер.порів");
    printf("─────────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < search_count; i++) {
        PerformanceResult result = measure_performance(balanced_tree, search_words[i]);

        printf("%-12s %-8s %-12.3f %-8d %-12.3f %-10d\n",
               search_words[i],
               result.found ? "Так" : "Ні",
               result.recursive_time * 1000000,  // Конвертуємо в мікросекунди
               result.recursive_calls_count,
               result.iterative_time * 1000000,
               result.iterative_comparisons_count);
    }

    freetree(balanced_tree);
}

void demonstrate_tree_shapes() {
    printf("\n=== ВПЛИВ ФОРМИ ДЕРЕВА НА ПРОДУКТИВНІСТЬ ===\n");

    char words[][20] = {
        "word01", "word02", "word03", "word04", "word05",
        "word06", "word07", "word08", "word09", "word10"
    };
    int word_count = sizeof(words) / sizeof(words[0]);

    // Збалансоване дерево
    printf("\n--- Збалансоване дерево ---\n");
    Treenode *balanced = create_balanced_tree(words, 0, word_count - 1);
    printf("Висота: %d\n", tree_height(balanced));

    PerformanceResult bal_result = measure_performance(balanced, "word08");
    printf("Пошук 'word08':\n");
    printf("  Рекурсивно: %d викликів\n", bal_result.recursive_calls_count);
    printf("  Ітераційно: %d порівнянь\n", bal_result.iterative_comparisons_count);

    // Незбалансоване (лінійне) дерево
    printf("\n--- Незбалансоване (лінійне) дерево ---\n");
    Treenode *linear = create_linear_tree(words, word_count);
    printf("Висота: %d\n", tree_height(linear));

    PerformanceResult lin_result = measure_performance(linear, "word08");
    printf("Пошук 'word08':\n");
    printf("  Рекурсивно: %d викликів\n", lin_result.recursive_calls_count);
    printf("  Ітераційно: %d порівнянь\n", lin_result.iterative_comparisons_count);

    printf("\nПорівняння:\n");
    printf("  Збалансоване: %d операцій\n", bal_result.recursive_calls_count);
    printf("  Лінійне: %d операцій\n", lin_result.recursive_calls_count);
    printf("  Різниця: у %.1f разів\n",
           (double)lin_result.recursive_calls_count / bal_result.recursive_calls_count);

    freetree(balanced);
    freetree(linear);
}

void stress_test() {
    printf("\n=== СТРЕС-ТЕСТУВАННЯ НА ВЕЛИКИХ ДАНИХ ===\n");

    const int LARGE_SIZE = 10000;
    const int SEARCH_ITERATIONS = 1000;

    printf("Створюємо дерево з %d елементів...\n", LARGE_SIZE);

    // Створюємо велике дерево
    Treenode *large_tree = NULL;
    for (int i = 0; i < LARGE_SIZE; i++) {
        char word[20];
        sprintf(word, "item%05d", rand() % (LARGE_SIZE * 2)); // Випадкові значення
        large_tree = insert(large_tree, word, 1);
    }

    printf("Висота дерева: %d\n", tree_height(large_tree));

    // Підготовка для тестування
    char search_word[20];
    sprintf(search_word, "item%05d", rand() % (LARGE_SIZE * 2));

    clock_t start, end;

    // Тест рекурсивної версії
    printf("\nТестування рекурсивної версії (%d пошуків)...\n", SEARCH_ITERATIONS);
    start = clock();
    int total_recursive_calls = 0;

    for (int i = 0; i < SEARCH_ITERATIONS; i++) {
        sprintf(search_word, "item%05d", rand() % (LARGE_SIZE * 2));
        recursive_calls = 0;
        lookup_recursive(large_tree, search_word);
        total_recursive_calls += recursive_calls;
    }
    end = clock();

    double recursive_total_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Тест ітераційної версії
    printf("Тестування ітераційної версії (%d пошуків)...\n", SEARCH_ITERATIONS);
    start = clock();
    int total_iterative_comparisons = 0;

    for (int i = 0; i < SEARCH_ITERATIONS; i++) {
        sprintf(search_word, "item%05d", rand() % (LARGE_SIZE * 2));
        lookup_iterative(large_tree, search_word);
        total_iterative_comparisons += iterative_comparisons;
    }
    end = clock();

    double iterative_total_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Результати
    printf("\nРезультати стрес-тестування:\n");
    printf("Рекурсивна версія:\n");
    printf("  Загальний час: %.6f сек\n", recursive_total_time);
    printf("  Час на пошук: %.6f сек\n", recursive_total_time / SEARCH_ITERATIONS);
    printf("  Середня кількість викликів: %.1f\n",
           (double)total_recursive_calls / SEARCH_ITERATIONS);

    printf("\nІтераційна версія:\n");
    printf("  Загальний час: %.6f сек\n", iterative_total_time);
    printf("  Час на пошук: %.6f сек\n", iterative_total_time / SEARCH_ITERATIONS);
    printf("  Середня кількість порівнянь: %.1f\n",
           (double)total_iterative_comparisons / SEARCH_ITERATIONS);

    printf("\nПорівняння швидкості:\n");
    if (iterative_total_time > 0) {
        printf("  Рекурсивна швидша у %.2f разів\n",
               iterative_total_time / recursive_total_time);
    }

    freetree(large_tree);
}

void memory_usage_analysis() {
    printf("\n=== АНАЛІЗ ВИКОРИСТАННЯ ПАМ'ЯТІ ===\n");

    printf("РЕКУРСИВНА ВЕРСІЯ (lookup):\n");
    printf("+ Простота коду\n");
    printf("+ Природність для рекурсивних структур\n");
    printf("- Використовує стек викликів\n");
    printf("- Ризик stack overflow для глибоких дерев\n");
    printf("- Додаткові накладні витрати на виклики функцій\n\n");

    printf("ІТЕРАЦІЙНА ВЕРСІЯ (nvlookup):\n");
    printf("+ Константне використання пам'яті O(1)\n");
    printf("+ Безпечна для глибоких дерев\n");
    printf("+ Менші накладні витрати\n");
    printf("- Трохи більше коду\n");
    printf("- Потребує ручного управління циклом\n\n");

    int depth_sizes[] = {10, 100, 1000, 10000};
    int sizes_count = sizeof(depth_sizes) / sizeof(depth_sizes[0]);

    printf("Теоретичне використання стеку для різних глибин:\n");
    printf("%-10s %-15s %-15s\n", "Глибина", "Рекурсивна", "Ітераційна");
    printf("─────────────────────────────────────────────\n");

    for (int i = 0; i < sizes_count; i++) {
        int depth = depth_sizes[i];
        int recursive_stack = depth * 64; // Приблизно 64 байти на кадр стеку
        int iterative_stack = 16; // Тільки локальні змінні

        printf("%-10d %-15d %-15d\n", depth, recursive_stack, iterative_stack);
    }

    printf("\nВИСНОВОК:\n");
    printf("Для глибоких дерев (>1000 рівнів) ітераційна версія безпечніша\n");
}

void practical_recommendations() {
    printf("\n=== ПРАКТИЧНІ РЕКОМЕНДАЦІЇ ===\n");

    printf("КОЛИ ВИКОРИСТОВУВАТИ РЕКУРСИВНУ ВЕРСІЮ:\n");
    printf("• Невеликі дерева (глибина < 1000)\n");
    printf("• Прототипування та навчальні цілі\n");
    printf("• Коли важлива читабельність коду\n");
    printf("• Інтеграція з іншими рекурсивними алгоритмами\n\n");

    printf("КОЛИ ВИКОРИСТОВУВАТИ ІТЕРАЦІЙНУ ВЕРСІЮ:\n");
    printf("• Великі або потенційно глибокі дерева\n");
    printf("• Продакшн системи з вимогами до стабільності\n");
    printf("• Обмежена пам'ять стеку\n");
    printf("• Критична продуктивність\n\n");

    printf("ОПТИМІЗАЦІЇ:\n");
    printf("• Використовуйте збалансовані дерева (AVL, RB)\n");
    printf("• Кешуйте часто шукані елементи\n");
    printf("• Розглядайте B-дерева для даних на диску\n");
    printf("• Моніторьте глибину дерева в runtime\n");
}

int main(void) {
    printf("=== Завдання 15: Порівняння lookup та nvlookup ===\n");

    srand(time(NULL));

    demonstrate_basic_comparison();
    demonstrate_tree_shapes();
    stress_test();
    memory_usage_analysis();
    practical_recommendations();

    return 0;
}