#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
ЗАВДАННЯ 16: Функція сортування з симетричним обходом

Реалізуємо сортування через бінарне дерево пошуку з симетричним обходом
та порівнюємо з іншими алгоритмами сортування.
*/

// ============= СТРУКТУРА ДЕРЕВА =============

typedef struct Treenode Treenode;

struct Treenode {
    char *word;
    int count;
    Treenode *left;
    Treenode *right;
};

// ============= ОСНОВНІ ФУНКЦІЇ ДЕРЕВА =============

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
        treep->count += count;  // Збільшуємо лічильник для дублікатів
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

// ============= СИМЕТРИЧНИЙ ОБХІД (IN-ORDER TRAVERSAL) =============

// Базовий симетричний обхід з виведенням
void inorder_print(Treenode *treep) {
    if (treep == NULL) return;

    inorder_print(treep->left);
    printf("%s ", treep->word);
    inorder_print(treep->right);
}

// Симетричний обхід з записом у масив
void inorder_to_array(Treenode *treep, char **result, int *index) {
    if (treep == NULL) return;

    inorder_to_array(treep->left, result, index);

    // Додаємо елемент стільки разів, скільки він зустрічається
    for (int i = 0; i < treep->count; i++) {
        result[*index] = (char *) malloc(strlen(treep->word) + 1);
        strcpy(result[*index], treep->word);
        (*index)++;
    }

    inorder_to_array(treep->right, result, index);
}

// Симетричний обхід з викликом функції для кожного елемента
void inorder_apply(Treenode *treep, void (*func)(char *word, int count, void *arg), void *arg) {
    if (treep == NULL) return;

    inorder_apply(treep->left, func, arg);
    func(treep->word, treep->count, arg);
    inorder_apply(treep->right, func, arg);
}

// ============= СОРТУВАННЯ ЧЕРЕЗ ДЕРЕВО =============

// Основна функція сортування через BST
char** tree_sort(char **words, int n, int *result_count) {
    if (words == NULL || n <= 0) {
        *result_count = 0;
        return NULL;
    }

    // Крок 1: Побудова дерева
    Treenode *tree = NULL;
    for (int i = 0; i < n; i++) {
        tree = insert(tree, words[i], 1);
    }

    // Крок 2: Підрахунок загальної кількості елементів
    *result_count = n; // В нашому випадку кількість не змінюється

    // Крок 3: Виділення пам'яті для результату
    char **result = (char **) malloc(n * sizeof(char *));
    if (result == NULL) {
        freetree(tree);
        *result_count = 0;
        return NULL;
    }

    // Крок 4: Симетричний обхід з записом у масив
    int index = 0;
    inorder_to_array(tree, result, &index);

    // Крок 5: Очищення пам'яті дерева
    freetree(tree);

    return result;
}

// Функція для звільнення результату tree_sort
void free_sorted_array(char **array, int count) {
    if (array == NULL) return;

    for (int i = 0; i < count; i++) {
        free(array[i]);
    }
    free(array);
}

// ============= ПОРІВНЯННЯ З ІНШИМИ АЛГОРИТМАМИ =============

// Швидке сортування (quicksort)
int string_compare(const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

char** quick_sort(char **words, int n) {
    // Створюємо копію масиву
    char **result = (char **) malloc(n * sizeof(char *));
    if (result == NULL) return NULL;

    for (int i = 0; i < n; i++) {
        result[i] = (char *) malloc(strlen(words[i]) + 1);
        strcpy(result[i], words[i]);
    }

    // Сортуємо
    qsort(result, n, sizeof(char *), string_compare);

    return result;
}

// Сортування вставками (для порівняння)
char** insertion_sort(char **words, int n) {
    // Створюємо копію масиву
    char **result = (char **) malloc(n * sizeof(char *));
    if (result == NULL) return NULL;

    for (int i = 0; i < n; i++) {
        result[i] = (char *) malloc(strlen(words[i]) + 1);
        strcpy(result[i], words[i]);
    }

    // Сортування вставками
    for (int i = 1; i < n; i++) {
        char *key = result[i];
        int j = i - 1;

        while (j >= 0 && strcmp(result[j], key) > 0) {
            result[j + 1] = result[j];
            j--;
        }
        result[j + 1] = key;
    }

    return result;
}

// ============= ТЕСТУВАННЯ ПРОДУКТИВНОСТІ =============

typedef struct {
    double tree_sort_time;
    double quick_sort_time;
    double insertion_sort_time;
    int array_size;
    char scenario[50];
} PerformanceTest;

PerformanceTest measure_sorting_performance(char **words, int n, const char *scenario) {
    PerformanceTest result = {0};
    strcpy(result.scenario, scenario);
    result.array_size = n;

    clock_t start, end;

    // Тестування tree sort
    start = clock();
    int tree_count;
    char **tree_result = tree_sort(words, n, &tree_count);
    end = clock();
    result.tree_sort_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Тестування quicksort
    start = clock();
    char **quick_result = quick_sort(words, n);
    end = clock();
    result.quick_sort_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Тестування insertion sort (тільки для невеликих масивів)
    if (n <= 1000) {
        start = clock();
        char **insertion_result = insertion_sort(words, n);
        end = clock();
        result.insertion_sort_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        free_sorted_array(insertion_result, n);
    } else {
        result.insertion_sort_time = -1; // Позначаємо як не тестувалося
    }

    // Очищення пам'яті
    free_sorted_array(tree_result, tree_count);
    free_sorted_array(quick_result, n);

    return result;
}

// ============= ГЕНЕРАТОРИ ТЕСТОВИХ ДАНИХ =============

// Випадкові дані
char** generate_random_data(int n) {
    char **words = (char **) malloc(n * sizeof(char *));
    if (words == NULL) return NULL;

    for (int i = 0; i < n; i++) {
        words[i] = (char *) malloc(20);
        sprintf(words[i], "word%04d", rand() % (n * 2));
    }

    return words;
}

// Відсортовані дані (найгірший випадок для BST)
char** generate_sorted_data(int n) {
    char **words = (char **) malloc(n * sizeof(char *));
    if (words == NULL) return NULL;

    for (int i = 0; i < n; i++) {
        words[i] = (char *) malloc(20);
        sprintf(words[i], "word%04d", i);
    }

    return words;
}

// Зворотно відсортовані дані
char** generate_reverse_sorted_data(int n) {
    char **words = (char **) malloc(n * sizeof(char *));
    if (words == NULL) return NULL;

    for (int i = 0; i < n; i++) {
        words[i] = (char *) malloc(20);
        sprintf(words[i], "word%04d", n - i - 1);
    }

    return words;
}

// Майже відсортовані дані
char** generate_nearly_sorted_data(int n) {
    char **words = generate_sorted_data(n);
    if (words == NULL) return NULL;

    // Робимо кілька випадкових перестановок
    for (int i = 0; i < n / 10; i++) {
        int idx1 = rand() % n;
        int idx2 = rand() % n;

        char *temp = words[idx1];
        words[idx1] = words[idx2];
        words[idx2] = temp;
    }

    return words;
}

void free_test_data(char **words, int n) {
    if (words == NULL) return;

    for (int i = 0; i < n; i++) {
        free(words[i]);
    }
    free(words);
}

// ============= ДЕМОНСТРАЦІЇ =============

void demonstrate_basic_sorting() {
    printf("=== БАЗОВА ДЕМОНСТРАЦІЯ СОРТУВАННЯ ЧЕРЕЗ ДЕРЕВО ===\n");

    char *test_words[] = {
        "banana", "apple", "cherry", "date", "elderberry",
        "fig", "grape", "apple", "banana"  // З дублікатами
    };
    int n = sizeof(test_words) / sizeof(test_words[0]);

    printf("Вхідні дані: ");
    for (int i = 0; i < n; i++) {
        printf("%s ", test_words[i]);
    }
    printf("\n");

    // Створюємо дерево та виводимо симетричний обхід
    printf("\nБудуємо дерево та виконуємо симетричний обхід:\n");
    Treenode *tree = NULL;
    for (int i = 0; i < n; i++) {
        tree = insert(tree, test_words[i], 1);
    }

    printf("Відсортований порядок: ");
    inorder_print(tree);
    printf("\n");

    // Тестуємо функцію tree_sort
    int sorted_count;
    char **sorted = tree_sort(test_words, n, &sorted_count);

    printf("Результат tree_sort: ");
    for (int i = 0; i < sorted_count; i++) {
        printf("%s ", sorted[i]);
    }
    printf("\n");

    freetree(tree);
    free_sorted_array(sorted, sorted_count);
}

void analyze_complexity() {
    printf("\n=== АНАЛІЗ СКЛАДНОСТІ TREE SORT ===\n");

    printf("ТЕОРЕТИЧНА СКЛАДНІСТЬ:\n");
    printf("1. Побудова дерева: O(n log n) - середній випадок\n");
    printf("                    O(n²) - найгірший випадок (відсортовані дані)\n");
    printf("2. Симетричний обхід: O(n) - завжди\n");
    printf("3. Загальна складність: O(n log n) - середній випадок\n");
    printf("                        O(n²) - найгірший випадок\n\n");

    printf("ВИКОРИСТАННЯ ПАМ'ЯТІ:\n");
    printf("• Дерево: O(n) для зберігання вузлів\n");
    printf("• Стек рекурсії: O(log n) - середній, O(n) - найгірший\n");
    printf("• Результуючий масив: O(n)\n");
    printf("• Загалом: O(n) + простір для дерева\n\n");

    // Практичне тестування на різних розмірах
    printf("ПРАКТИЧНЕ ТЕСТУВАННЯ:\n");
    printf("%-8s %-12s %-12s %-12s\n", "Розмір", "Tree Sort", "Quick Sort", "Різниця");
    printf("─────────────────────────────────────────────────\n");

    int sizes[] = {100, 500, 1000, 2000};
    int size_count = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < size_count; i++) {
        char **random_data = generate_random_data(sizes[i]);
        PerformanceTest result = measure_sorting_performance(random_data, sizes[i], "random");

        double ratio = result.tree_sort_time / result.quick_sort_time;

        printf("%-8d %-12.6f %-12.6f %-12.2fx\n",
               sizes[i], result.tree_sort_time, result.quick_sort_time, ratio);

        free_test_data(random_data, sizes[i]);
    }
}

void test_worst_case_scenarios() {
    printf("\n=== ТЕСТУВАННЯ НАЙГІРШИХ СЦЕНАРІЇВ ===\n");

    const int TEST_SIZE = 1000;

    // Тест 1: Відсортовані дані
    printf("1. Відсортовані дані (найгірший випадок для BST):\n");
    char **sorted_data = generate_sorted_data(TEST_SIZE);
    PerformanceTest sorted_result = measure_sorting_performance(sorted_data, TEST_SIZE, "sorted");

    printf("   Tree Sort: %.6f сек\n", sorted_result.tree_sort_time);
    printf("   Quick Sort: %.6f сек\n", sorted_result.quick_sort_time);
    printf("   Tree Sort повільніше у %.2f разів\n",
           sorted_result.tree_sort_time / sorted_result.quick_sort_time);

    // Тест 2: Зворотно відсортовані дані
    printf("\n2. Зворотно відсортовані дані:\n");
    char **reverse_data = generate_reverse_sorted_data(TEST_SIZE);
    PerformanceTest reverse_result = measure_sorting_performance(reverse_data, TEST_SIZE, "reverse");

    printf("   Tree Sort: %.6f сек\n", reverse_result.tree_sort_time);
    printf("   Quick Sort: %.6f сек\n", reverse_result.quick_sort_time);
    printf("   Tree Sort повільніше у %.2f разів\n",
           reverse_result.tree_sort_time / reverse_result.quick_sort_time);

    // Тест 3: Випадкові дані (кращий випадок для BST)
    printf("\n3. Випадкові дані (кращий випадок для BST):\n");
    char **random_data = generate_random_data(TEST_SIZE);
    PerformanceTest random_result = measure_sorting_performance(random_data, TEST_SIZE, "random");

    printf("   Tree Sort: %.6f сек\n", random_result.tree_sort_time);
    printf("   Quick Sort: %.6f сек\n", random_result.quick_sort_time);
    printf("   Співвідношення: %.2f\n",
           random_result.tree_sort_time / random_result.quick_sort_time);

    printf("\nВИСНОВКИ:\n");
    printf("• Tree Sort ДУЖЕ повільний на відсортованих даних\n");
    printf("• Випадкові дані дають прийнятну продуктивність\n");
    printf("• Quick Sort стабільно швидший в усіх випадках\n");

    free_test_data(sorted_data, TEST_SIZE);
    free_test_data(reverse_data, TEST_SIZE);
    free_test_data(random_data, TEST_SIZE);
}

void compare_with_library_functions() {
    printf("\n=== ПОРІВНЯННЯ З БІБЛІОТЕЧНИМИ ФУНКЦІЯМИ ===\n");

    const int TEST_SIZE = 5000;
    char **test_data = generate_random_data(TEST_SIZE);

    printf("Тестування на %d випадкових елементах:\n\n", TEST_SIZE);

    PerformanceTest result = measure_sorting_performance(test_data, TEST_SIZE, "library_comparison");

    printf("Результати:\n");
    printf("• Tree Sort:      %.6f сек\n", result.tree_sort_time);
    printf("• Quick Sort:     %.6f сек\n", result.quick_sort_time);
    if (result.insertion_sort_time >= 0) {
        printf("• Insertion Sort: %.6f сек\n", result.insertion_sort_time);
    }

    printf("\nВідносна швидкість (Quick Sort = 1.0):\n");
    printf("• Tree Sort:      %.2fx\n", result.tree_sort_time / result.quick_sort_time);
    if (result.insertion_sort_time >= 0) {
        printf("• Insertion Sort: %.2fx\n", result.insertion_sort_time / result.quick_sort_time);
    }

    printf("\nХАРАКТЕРИСТИКИ АЛГОРИТМІВ:\n");
    printf("Tree Sort:\n");
    printf("  + Стабільний (зберігає порядок однакових елементів)\n");
    printf("  + Легко видаляти дублікати\n");
    printf("  + Природний для рекурсивних структур\n");
    printf("  - O(n²) в найгіршому випадку\n");
    printf("  - Додаткова пам'ять для дерева\n");
    printf("  - Не in-place\n\n");

    printf("Quick Sort (qsort):\n");
    printf("  + O(n log n) в середньому\n");
    printf("  + Оптимізований в бібліотеці\n");
    printf("  + Швидкий для більшості даних\n");
    printf("  - Не стабільний\n");
    printf("  - O(n²) в найгіршому випадку\n");
    printf("  - Потребує додаткову функцію порівняння\n");

    free_test_data(test_data, TEST_SIZE);
}

void when_tree_sort_fails() {
    printf("\n=== КОЛИ TREE SORT ПРАЦЮЄ ПОГАНО ===\n");

    printf("1. ВІДСОРТОВАНІ АБО МАЙЖЕ ВІДСОРТОВАНІ ДАНІ:\n");
    printf("   Причина: Дерево вироджується в список\n");
    printf("   Складність: O(n²) замість O(n log n)\n\n");

    printf("2. ВЕЛИКІ ОБСЯГИ ДАНИХ:\n");
    printf("   Причина: Додаткові накладні витрати на дерево\n");
    printf("   Проблема: Фрагментація пам'яті\n\n");

    printf("3. ОБМЕЖЕНА ПАМ'ЯТЬ:\n");
    printf("   Причина: Потребує O(n) додаткової пам'яті\n");
    printf("   Проблема: Стек рекурсії може переповнитися\n\n");

    printf("4. ЧАСТІ ОПЕРАЦІЇ СОРТУВАННЯ:\n");
    printf("   Причина: Побудова дерева займає час\n");
    printf("   Рішення: Використовувати готові алгоритми\n\n");

    printf("АЛЬТЕРНАТИВИ:\n");
    printf("• Merge Sort - стабільний O(n log n)\n");
    printf("• Heap Sort - гарантований O(n log n)\n");
    printf("• Quick Sort - швидкий в середньому\n");
    printf("• Radix Sort - для цілих чисел O(n)\n");
    printf("• Tim Sort - оптимізований для реальних даних\n");
}

int main(void) {
    printf("=== Завдання 16: Сортування з симетричним обходом ===\n");

    srand(time(NULL));

    demonstrate_basic_sorting();
    analyze_complexity();
    test_worst_case_scenarios();
    compare_with_library_functions();
    when_tree_sort_fails();

    printf("\n=== ЗАГАЛЬНІ ВИСНОВКИ ===\n");
    printf("ПОРЯДОК ШВИДКОДІЇ:\n");
    printf("• Середній випадок: O(n log n)\n");
    printf("• Найгірший випадок: O(n²) для відсортованих даних\n");
    printf("• Найкращий випадок: O(n log n) для збалансованих дерев\n\n");

    printf("КОЛИ ПРАЦЮЄ ПОГАНО:\n");
    printf("• Відсортовані або зворотно відсортовані дані\n");
    printf("• Великі обсяги даних з обмеженою пам'яттю\n");
    printf("• Коли потрібна максимальна швидкість\n\n");

    printf("ПОРІВНЯННЯ З QUICKSORT:\n");
    printf("• Tree Sort: стабільний, але повільніший\n");
    printf("• Quick Sort: швидший, але не стабільний\n");
    printf("• Бібліотечні функції оптимізовані та швидші\n\n");

    printf("ПРАКТИЧНІ РЕКОМЕНДАЦІЇ:\n");
    printf("• Використовуйте Tree Sort для навчальних цілей\n");
    printf("• Для продакшн коду застосовуйте qsort() або std::sort()\n");
    printf("• Tree Sort корисний коли потрібно видалити дублікати\n");
    printf("• Розгляньте самобалансуючі дерева для стабільної продуктивності\n");

    return 0;
}