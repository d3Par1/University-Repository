#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/*
ЗАВДАННЯ 17: Тестування функцій роботи з бінарними деревами

Комплексна система тестування для функцій з прикладів
"Binary search tree" та "Tree-traverser":
- Вставка, пошук, видалення
- Різні види обходу дерева
- Крайні випадки та стрес-тестування
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
        treep->count += count;
    } else if (cmp < 0) {
        treep->left = insert(treep->left, word, count);
    } else {
        treep->right = insert(treep->right, word, count);
    }

    return treep;
}

Treenode* lookup(Treenode *treep, char *word) {
    int cmp;

    if (treep == NULL)
        return NULL;

    cmp = strcmp(word, treep->word);
    if (cmp == 0)
        return treep;
    else if (cmp < 0)
        return lookup(treep->left, word);
    else
        return lookup(treep->right, word);
}

// Функція видалення вузла
Treenode* find_min(Treenode *treep) {
    while (treep && treep->left) {
        treep = treep->left;
    }
    return treep;
}

Treenode* delete_node(Treenode *treep, char *word) {
    if (treep == NULL) return NULL;

    int cmp = strcmp(word, treep->word);

    if (cmp < 0) {
        treep->left = delete_node(treep->left, word);
    } else if (cmp > 0) {
        treep->right = delete_node(treep->right, word);
    } else {
        // Знайшли вузол для видалення
        if (treep->left == NULL) {
            Treenode *temp = treep->right;
            free(treep->word);
            free(treep);
            return temp;
        } else if (treep->right == NULL) {
            Treenode *temp = treep->left;
            free(treep->word);
            free(treep);
            return temp;
        }

        // Вузол з двома дітьми
        Treenode *temp = find_min(treep->right);

        // Копіюємо дані
        free(treep->word);
        treep->word = (char *) malloc(strlen(temp->word) + 1);
        strcpy(treep->word, temp->word);
        treep->count = temp->count;

        // Видаляємо наступника
        treep->right = delete_node(treep->right, temp->word);
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

// ============= ФУНКЦІЇ ОБХОДУ =============

// Симетричний обхід (in-order)
void inorder_traverse(Treenode *treep, char **result, int *index) {
    if (treep == NULL) return;

    inorder_traverse(treep->left, result, index);
    result[*index] = treep->word;
    (*index)++;
    inorder_traverse(treep->right, result, index);
}

// Прямий обхід (pre-order)
void preorder_traverse(Treenode *treep, char **result, int *index) {
    if (treep == NULL) return;

    result[*index] = treep->word;
    (*index)++;
    preorder_traverse(treep->left, result, index);
    preorder_traverse(treep->right, result, index);
}

// Зворотний обхід (post-order)
void postorder_traverse(Treenode *treep, char **result, int *index) {
    if (treep == NULL) return;

    postorder_traverse(treep->left, result, index);
    postorder_traverse(treep->right, result, index);
    result[*index] = treep->word;
    (*index)++;
}

// ============= ДОПОМІЖНІ ФУНКЦІЇ ДЛЯ ТЕСТУВАННЯ =============

int tree_size(Treenode *treep) {
    if (treep == NULL) return 0;
    return 1 + tree_size(treep->left) + tree_size(treep->right);
}

int tree_height(Treenode *treep) {
    if (treep == NULL) return 0;

    int left_height = tree_height(treep->left);
    int right_height = tree_height(treep->right);

    return 1 + (left_height > right_height ? left_height : right_height);
}

int is_valid_bst(Treenode *treep, char *min_val, char *max_val) {
    if (treep == NULL) return 1;

    // Перевіряємо поточний вузол
    if ((min_val && strcmp(treep->word, min_val) <= 0) ||
        (max_val && strcmp(treep->word, max_val) >= 0)) {
        return 0;
    }

    // Рекурсивно перевіряємо піддерева
    return is_valid_bst(treep->left, min_val, treep->word) &&
           is_valid_bst(treep->right, treep->word, max_val);
}

// ============= ТЕСТУВАЛЬНА СИСТЕМА =============

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

// ============= ТЕСТИ ОСНОВНИХ ФУНКЦІЙ =============

void test_tree_creation() {
    test_start("Створення вузла дерева");

    Treenode *node = newtree("test", 5);
    int result = (node != NULL &&
                  strcmp(node->word, "test") == 0 &&
                  node->count == 5 &&
                  node->left == NULL &&
                  node->right == NULL);

    test_assert(result, "Вузол створений некоректно");

    if (node) {
        free(node->word);
        free(node);
    }
}

void test_insertion_and_lookup() {
    test_start("Вставка та пошук елементів");

    Treenode *tree = NULL;

    // Вставляємо елементи
    tree = insert(tree, "dog", 1);
    tree = insert(tree, "cat", 1);
    tree = insert(tree, "elephant", 1);
    tree = insert(tree, "bird", 1);

    // Перевіряємо пошук
    Treenode *found_dog = lookup(tree, "dog");
    Treenode *found_cat = lookup(tree, "cat");
    Treenode *found_missing = lookup(tree, "zebra");

    int result = (found_dog != NULL && strcmp(found_dog->word, "dog") == 0 &&
                  found_cat != NULL && strcmp(found_cat->word, "cat") == 0 &&
                  found_missing == NULL);

    test_assert(result, "Вставка або пошук працює некоректно");

    freetree(tree);
}

void test_duplicate_insertion() {
    test_start("Вставка дублікатів");

    Treenode *tree = NULL;
    tree = insert(tree, "word", 1);
    tree = insert(tree, "word", 3);
    tree = insert(tree, "word", 2);

    Treenode *found = lookup(tree, "word");
    int result = (found != NULL && found->count == 6); // 1+3+2

    test_assert(result, "Дублікати обробляються некоректно");

    freetree(tree);
}

void test_tree_structure() {
    test_start("Структура BST");

    Treenode *tree = NULL;
    char *words[] = {"dog", "cat", "elephant", "bird", "fish"};
    int word_count = sizeof(words) / sizeof(words[0]);

    for (int i = 0; i < word_count; i++) {
        tree = insert(tree, words[i], 1);
    }

    // Перевіряємо, чи дерево є валідним BST
    int is_valid = is_valid_bst(tree, NULL, NULL);
    int size = tree_size(tree);

    int result = (is_valid && size == word_count);

    test_assert(result, "Структура BST порушена");

    freetree(tree);
}

void test_deletion() {
    test_start("Видалення вузлів");

    Treenode *tree = NULL;
    char *words[] = {"dog", "cat", "elephant", "bird", "fish", "ant", "zebra"};
    int word_count = sizeof(words) / sizeof(words[0]);

    // Створюємо дерево
    for (int i = 0; i < word_count; i++) {
        tree = insert(tree, words[i], 1);
    }

    int original_size = tree_size(tree);

    // Видаляємо листок
    tree = delete_node(tree, "ant");

    // Видаляємо вузол з одним дитям
    tree = delete_node(tree, "zebra");

    // Видаляємо вузол з двома дітьми
    tree = delete_node(tree, "dog");

    int new_size = tree_size(tree);
    int still_valid = is_valid_bst(tree, NULL, NULL);

    // Перевіряємо, що видалені елементи відсутні
    int ant_absent = (lookup(tree, "ant") == NULL);
    int zebra_absent = (lookup(tree, "zebra") == NULL);
    int dog_absent = (lookup(tree, "dog") == NULL);

    int result = (new_size == original_size - 3 &&
                  still_valid &&
                  ant_absent && zebra_absent && dog_absent);

    test_assert(result, "Видалення працює некоректно");

    freetree(tree);
}

// ============= ТЕСТИ ОБХОДУ =============

void test_inorder_traversal() {
    test_start("Симетричний обхід (in-order)");

    Treenode *tree = NULL;
    char *words[] = {"dog", "cat", "elephant", "bird"};
    int word_count = sizeof(words) / sizeof(words[0]);

    for (int i = 0; i < word_count; i++) {
        tree = insert(tree, words[i], 1);
    }

    char *result[10];
    int index = 0;
    inorder_traverse(tree, result, &index);

    // In-order обхід BST повинен дати відсортований порядок
    int is_sorted = 1;
    for (int i = 1; i < index; i++) {
        if (strcmp(result[i-1], result[i]) >= 0) {
            is_sorted = 0;
            break;
        }
    }

    test_assert(is_sorted && index == word_count,
                "In-order обхід не дає відсортованого порядку");

    freetree(tree);
}

void test_preorder_traversal() {
    test_start("Прямий обхід (pre-order)");

    // Створюємо просте дерево для перевірки порядку
    Treenode *tree = newtree("B", 1);
    tree->left = newtree("A", 1);
    tree->right = newtree("C", 1);

    char *result[10];
    int index = 0;
    preorder_traverse(tree, result, &index);

    // Pre-order: корінь, ліве, праве -> B, A, C
    int correct_order = (index == 3 &&
                        strcmp(result[0], "B") == 0 &&
                        strcmp(result[1], "A") == 0 &&
                        strcmp(result[2], "C") == 0);

    test_assert(correct_order, "Pre-order обхід працює некоректно");

    freetree(tree);
}

void test_postorder_traversal() {
    test_start("Зворотний обхід (post-order)");

    // Створюємо просте дерево для перевірки порядку
    Treenode *tree = newtree("B", 1);
    tree->left = newtree("A", 1);
    tree->right = newtree("C", 1);

    char *result[10];
    int index = 0;
    postorder_traverse(tree, result, &index);

    // Post-order: ліве, праве, корінь -> A, C, B
    int correct_order = (index == 3 &&
                        strcmp(result[0], "A") == 0 &&
                        strcmp(result[1], "C") == 0 &&
                        strcmp(result[2], "B") == 0);

    test_assert(correct_order, "Post-order обхід працює некоректно");

    freetree(tree);
}

// ============= ТЕСТИ КРАЙНІХ ВИПАДКІВ =============

void test_empty_tree() {
    test_start("Операції з порожнім деревом");

    Treenode *empty_tree = NULL;

    // Пошук в порожньому дереві
    Treenode *found = lookup(empty_tree, "anything");

    // Видалення з порожнього дерева
    empty_tree = delete_node(empty_tree, "anything");

    // Розмір порожнього дерева
    int size = tree_size(empty_tree);
    int height = tree_height(empty_tree);

    int result = (found == NULL && empty_tree == NULL &&
                  size == 0 && height == 0);

    test_assert(result, "Операції з порожнім деревом працюють некоректно");
}

void test_single_node() {
    test_start("Дерево з одним вузлом");

    Treenode *tree = newtree("single", 1);

    // Пошук
    Treenode *found = lookup(tree, "single");
    Treenode *not_found = lookup(tree, "other");

    // Розмір та висота
    int size = tree_size(tree);
    int height = tree_height(tree);

    // Валідність BST
    int is_valid = is_valid_bst(tree, NULL, NULL);

    int result = (found == tree && not_found == NULL &&
                  size == 1 && height == 1 && is_valid);

    test_assert(result, "Дерево з одним вузлом працює некоректно");

    // Видалення єдиного вузла
    tree = delete_node(tree, "single");
    int empty_after_delete = (tree == NULL);

    test_start("Видалення єдиного вузла");
    test_assert(empty_after_delete, "Видалення єдиного вузла залишає дерево не порожнім");
}

void test_linear_tree() {
    test_start("Лінійне дерево (найгірший випадок)");

    Treenode *tree = NULL;

    // Створюємо лінійне дерево (тільки праві діти)
    for (int i = 1; i <= 5; i++) {
        char word[10];
        sprintf(word, "word%d", i);
        tree = insert(tree, word, 1);
    }

    int size = tree_size(tree);
    int height = tree_height(tree);
    int is_valid = is_valid_bst(tree, NULL, NULL);

    // Лінійне дерево має висоту рівну розміру
    int result = (size == 5 && height == 5 && is_valid);

    test_assert(result, "Лінійне дерево обробляється некоректно");

    freetree(tree);
}

// ============= СТРЕС-ТЕСТУВАННЯ =============

void stress_test_insertion() {
    test_start("Стрес-тест вставки (1000 елементів)");

    Treenode *tree = NULL;
    const int COUNT = 1000;

    // Вставляємо багато елементів
    for (int i = 0; i < COUNT; i++) {
        char word[20];
        sprintf(word, "item%04d", rand() % (COUNT * 2));
        tree = insert(tree, word, 1);
    }

    int size = tree_size(tree);
    int is_valid = is_valid_bst(tree, NULL, NULL);

    int result = (size > 0 && size <= COUNT && is_valid);

    test_assert(result, "Стрес-тест вставки провалився");

    freetree(tree);
}

void stress_test_mixed_operations() {
    test_start("Стрес-тест змішаних операцій");

    Treenode *tree = NULL;
    const int OPERATIONS = 500;
    int operations_passed = 0;

    for (int i = 0; i < OPERATIONS; i++) {
        char word[20];
        sprintf(word, "item%03d", rand() % 100);

        int operation = rand() % 3;

        switch (operation) {
            case 0: // Вставка
                tree = insert(tree, word, 1);
                operations_passed++;
                break;

            case 1: // Пошук
                lookup(tree, word);
                operations_passed++;
                break;

            case 2: // Видалення
                tree = delete_node(tree, word);
                operations_passed++;
                break;
        }

        // Перевіряємо валідність після кожної операції
        if (!is_valid_bst(tree, NULL, NULL)) {
            break;
        }
    }

    int result = (operations_passed == OPERATIONS &&
                  is_valid_bst(tree, NULL, NULL));

    test_assert(result, "Стрес-тест змішаних операцій провалився");

    freetree(tree);
}

// ============= ТЕСТ ПРОДУКТИВНОСТІ =============

void performance_test() {
    printf("\n=== ТЕСТИ ПРОДУКТИВНОСТІ ===\n");

    const int SIZES[] = {100, 500, 1000, 2000};
    const int SIZE_COUNT = sizeof(SIZES) / sizeof(SIZES[0]);

    printf("%-8s %-12s %-12s %-12s %-10s\n",
           "Розмір", "Вставка(с)", "Пошук(с)", "Висота", "Валідний");
    printf("─────────────────────────────────────────────────────────\n");

    for (int s = 0; s < SIZE_COUNT; s++) {
        int size = SIZES[s];
        Treenode *tree = NULL;

        // Тест вставки
        clock_t start = clock();
        for (int i = 0; i < size; i++) {
            char word[20];
            sprintf(word, "item%05d", rand() % (size * 2));
            tree = insert(tree, word, 1);
        }
        clock_t end = clock();
        double insert_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        // Тест пошуку
        start = clock();
        for (int i = 0; i < size / 10; i++) {
            char word[20];
            sprintf(word, "item%05d", rand() % (size * 2));
            lookup(tree, word);
        }
        end = clock();
        double search_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        int height = tree_height(tree);
        int is_valid = is_valid_bst(tree, NULL, NULL);

        printf("%-8d %-12.6f %-12.6f %-12d %-10s\n",
               size, insert_time, search_time, height,
               is_valid ? "Так" : "Ні");

        freetree(tree);
    }
}

// ============= ГОЛОВНА ФУНКЦІЯ ТЕСТУВАННЯ =============

void run_all_tests() {
    printf("=== ЗАПУСК ПОВНОГО НАБОРУ ТЕСТІВ ===\n\n");

    // Тести основних функцій
    printf("=== ТЕСТИ ОСНОВНИХ ФУНКЦІЙ ===\n");
    test_tree_creation();
    test_insertion_and_lookup();
    test_duplicate_insertion();
    test_tree_structure();
    test_deletion();

    // Тести обходу
    printf("\n=== ТЕСТИ ОБХОДУ ДЕРЕВА ===\n");
    test_inorder_traversal();
    test_preorder_traversal();
    test_postorder_traversal();

    // Тести крайніх випадків
    printf("\n=== ТЕСТИ КРАЙНІХ ВИПАДКІВ ===\n");
    test_empty_tree();
    test_single_node();
    test_linear_tree();

    // Стрес-тестування
    printf("\n=== СТРЕС-ТЕСТУВАННЯ ===\n");
    stress_test_insertion();
    stress_test_mixed_operations();

    // Підсумок
    test_summary();

    // Тести продуктивності
    performance_test();
}

int main(void) {
    printf("=== Завдання 17: Тестування функцій роботи з деревами ===\n\n");

    srand(time(NULL));

    run_all_tests();

    printf("\n=== РЕКОМЕНДАЦІЇ ПО ТЕСТУВАННЮ ДЕРЕВ ===\n");
    printf("1. ОСНОВНІ ФУНКЦІЇ:\n");
    printf("   • Тестуйте створення, вставку, пошук, видалення\n");
    printf("   • Перевіряйте обробку дублікатів\n");
    printf("   • Валідуйте структуру BST після кожної операції\n\n");

    printf("2. ОБХОДИ ДЕРЕВА:\n");
    printf("   • In-order повинен давати відсортований порядок\n");
    printf("   • Pre-order та post-order мають специфічні порядки\n");
    printf("   • Тестуйте на різних формах дерев\n\n");

    printf("3. КРАЙНІ ВИПАДКИ:\n");
    printf("   • Порожнє дерево\n");
    printf("   • Дерево з одним вузлом\n");
    printf("   • Лінійне дерево (найгірший випадок)\n");
    printf("   • Збалансоване дерево (найкращий випадок)\n\n");

    printf("4. СТРЕС-ТЕСТУВАННЯ:\n");
    printf("   • Великі обсяги даних\n");
    printf("   • Змішані операції\n");
    printf("   • Випадкові та патологічні дані\n\n");

    printf("5. ПРОДУКТИВНІСТЬ:\n");
    printf("   • Вимірюйте час операцій\n");
    printf("   • Перевіряйте висоту дерева\n");
    printf("   • Моніторьте використання пам'яті\n\n");

    printf("6. ІНСТРУМЕНТИ:\n");
    printf("   • valgrind для перевірки пам'яті\n");
    printf("   • Unit testing frameworks\n");
    printf("   • Автоматизація тестів\n");
    printf("   • Continuous integration\n");

    return (test_suite.failed_tests == 0) ? 0 : 1;
}