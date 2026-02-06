#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


typedef struct Treenode Treenode;

struct Treenode {
    char *word;
    int count;
    Treenode *left;
    Treenode *right;
};


// Створення нового вузла
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

// Вставка елемента в дерево
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

// Пошук елемента в дереві
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

// Звільнення пам'яті дерева
void freetree(Treenode *treep) {
    if (treep == NULL) return;

    freetree(treep->left);
    freetree(treep->right);
    free(treep->word);
    free(treep);
}


// Обчислення висоти дерева
int tree_height(Treenode *treep) {
    if (treep == NULL) return 0;

    int left_height = tree_height(treep->left);
    int right_height = tree_height(treep->right);

    return 1 + (left_height > right_height ? left_height : right_height);
}

// Підрахунок кількості вузлів
int tree_size(Treenode *treep) {
    if (treep == NULL) return 0;
    return 1 + tree_size(treep->left) + tree_size(treep->right);
}

// Обчислення мінімальної та максимальної глибини
void depth_analysis(Treenode *treep, int current_depth, int *min_depth, int *max_depth) {
    if (treep == NULL) {
        if (*min_depth == -1 || current_depth < *min_depth) {
            *min_depth = current_depth;
        }
        if (current_depth > *max_depth) {
            *max_depth = current_depth;
        }
        return;
    }

    // Якщо це листок
    if (treep->left == NULL && treep->right == NULL) {
        if (*min_depth == -1 || current_depth < *min_depth) {
            *min_depth = current_depth;
        }
        if (current_depth > *max_depth) {
            *max_depth = current_depth;
        }
        return;
    }

    if (treep->left != NULL) {
        depth_analysis(treep->left, current_depth + 1, min_depth, max_depth);
    }
    if (treep->right != NULL) {
        depth_analysis(treep->right, current_depth + 1, min_depth, max_depth);
    }
}

// Розрахунок коефіцієнта збалансованості
typedef struct {
    int size;
    int height;
    int min_depth;
    int max_depth;
    double balance_factor;
    double ideal_height;
    int is_balanced;
    char balance_description[200];
} BalanceInfo;

BalanceInfo analyze_balance(Treenode *treep) {
    BalanceInfo info = {0};

    if (treep == NULL) {
        strcpy(info.balance_description, "Порожнє дерево");
        info.is_balanced = 1;
        return info;
    }

    info.size = tree_size(treep);
    info.height = tree_height(treep);
    info.min_depth = -1;
    info.max_depth = 0;

    depth_analysis(treep, 0, &info.min_depth, &info.max_depth);

    // Ідеальна висота для збалансованого дерева
    info.ideal_height = log2(info.size + 1);

    // Коефіцієнт збалансованості (чим ближче до 1, тим краще)
    info.balance_factor = info.ideal_height / info.height;

    // Визначення рівня збалансованості
    if (info.balance_factor >= 0.9) {
        info.is_balanced = 1;
        strcpy(info.balance_description, "Добре збалансоване");
    } else if (info.balance_factor >= 0.7) {
        info.is_balanced = 1;
        strcpy(info.balance_description, "Помірно збалансоване");
    } else if (info.balance_factor >= 0.5) {
        info.is_balanced = 0;
        strcpy(info.balance_description, "Слабко збалансоване");
    } else {
        info.is_balanced = 0;
        strcpy(info.balance_description, "Незбалансоване (схоже на список)");
    }

    return info;
}

// Виведення детальної інформації про збалансованість
void print_balance_info(BalanceInfo info, const char *title) {
    printf("\n=== %s ===\n", title);
    printf("Кількість вузлів: %d\n", info.size);
    printf("Висота дерева: %d\n", info.height);
    printf("Мінімальна глибина листків: %d\n", info.min_depth);
    printf("Максимальна глибина листків: %d\n", info.max_depth);
    printf("Ідеальна висота: %.2f\n", info.ideal_height);
    printf("Коефіцієнт збалансованості: %.3f\n", info.balance_factor);
    printf("Оцінка: %s\n", info.balance_description);

    if (info.size > 0) {
        printf("Ефективність пошуку: O(%.1f) замість ідеального O(%.1f)\n",
               (double)info.height, info.ideal_height);
    }
}


// Генератор випадкових слів
void generate_random_word(char *word, int max_length) {
    int length = 3 + rand() % (max_length - 3); // Довжина від 3 до max_length

    for (int i = 0; i < length; i++) {
        word[i] = 'a' + rand() % 26;
    }
    word[length] = '\0';
}

// Генерація випадкового набору даних
Treenode* generate_random_tree(int count) {
    printf("Генеруємо дерево з %d випадкових елементів...\n", count);

    Treenode *tree = NULL;
    char word[20];

    for (int i = 0; i < count; i++) {
        generate_random_word(word, 15);
        tree = insert(tree, word, 1);

        if (i % (count / 10) == 0 || count < 20) {
            printf("  Додано: %s (елемент %d/%d)\n", word, i + 1, count);
        }
    }

    return tree;
}

// Генерація відсортованого набору даних
Treenode* generate_sorted_tree(int count) {
    printf("Генеруємо дерево з %d відсортованих елементів...\n", count);

    Treenode *tree = NULL;
    char word[20];

    for (int i = 0; i < count; i++) {
        sprintf(word, "word%04d", i);  // word0000, word0001, ...
        tree = insert(tree, word, 1);

        if (i % (count / 10) == 0 || count < 20) {
            printf("  Додано: %s (елемент %d/%d)\n", word, i + 1, count);
        }
    }

    return tree;
}


// Виведення структури дерева (спрощена версія)
void print_tree_structure(Treenode *treep, int depth, char *prefix) {
    if (treep == NULL) return;

    if (depth < 4) { // Обмежуємо глибину для читабельності
        printf("%s%s%s (count: %d)\n", prefix,
               (depth > 0) ? "├── " : "", treep->word, treep->count);

        if (treep->left != NULL || treep->right != NULL) {
            char new_prefix[100];
            sprintf(new_prefix, "%s%s", prefix, (depth > 0) ? "│   " : "");

            if (treep->left != NULL) {
                print_tree_structure(treep->left, depth + 1, new_prefix);
            }
            if (treep->right != NULL) {
                print_tree_structure(treep->right, depth + 1, new_prefix);
            }
        }
    } else if (depth == 4) {
        printf("%s├── ... (більше вузлів на глибині %d+)\n", prefix, depth);
    }
}


void demonstrate_random_tree() {
    printf("\n=== ВАРІАНТ 1: ВИПАДКОВИЙ ПОРЯДОК ЕЛЕМЕНТІВ ===\n");

    srand(time(NULL));

    // Тестуємо різні розміри
    int sizes[] = {10, 50, 100, 500};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        printf("\n--- Тест з %d випадковими елементами ---\n", sizes[i]);

        Treenode *tree = generate_random_tree(sizes[i]);
        BalanceInfo info = analyze_balance(tree);

        print_balance_info(info, "Аналіз випадкового дерева");

        if (sizes[i] <= 20) {
            printf("\nСтруктура дерева:\n");
            print_tree_structure(tree, 0, "");
        }

        freetree(tree);
    }

    printf("\n=== ВИСНОВКИ ДЛЯ ВИПАДКОВИХ ДАНИХ ===\n");
    printf("Твердження: 'Якщо елементи надходять у випадковому порядку,\n");
    printf("то дерево буде більш-менш збалансованим'\n\n");
    printf("РЕЗУЛЬТАТ: ЧАСТКОВО ПІДТВЕРДЖЕНО\n");
    printf("• Випадкові дані дають кращу збалансованість ніж відсортовані\n");
    printf("• Але збалансованість не гарантована і може варіюватися\n");
    printf("• В середньому висота близька до O(log n), але може бути гірше\n");
    printf("• Великі дерева мають тенденцію до кращої збалансованості\n");
}

void demonstrate_sorted_tree() {
    printf("\n\n=== ВАРІАНТ 2: ВІДСОРТОВАНИЙ ПОРЯДОК ЕЛЕМЕНТІВ ===\n");

    // Тестуємо різні розміри
    int sizes[] = {10, 50, 100, 200};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        printf("\n--- Тест з %d відсортованими елементами ---\n", sizes[i]);

        Treenode *tree = generate_sorted_tree(sizes[i]);
        BalanceInfo info = analyze_balance(tree);

        print_balance_info(info, "Аналіз відсортованого дерева");

        if (sizes[i] <= 20) {
            printf("\nСтруктура дерева:\n");
            print_tree_structure(tree, 0, "");
        }

        freetree(tree);
    }

    printf("\n=== ВИСНОВКИ ДЛЯ ВІДСОРТОВАНИХ ДАНИХ ===\n");
    printf("Твердження: 'Якщо елементи прибувають у відсортованому вигляді,\n");
    printf("то спуск завжди буде до самого низу однієї гілки,\n");
    printf("представляючи собою список за вказівником right'\n\n");
    printf("РЕЗУЛЬТАТ: ПОВНІСТЮ ПІДТВЕРДЖЕНО\n");
    printf("• Відсортовані дані створюють вироджене дерево\n");
    printf("• Структура стає лінійним списком (тільки праві діти)\n");
    printf("• Висота дорівнює кількості елементів O(n)\n");
    printf("• Ефективність пошуку деградує до O(n) замість O(log n)\n");
    printf("• Це найгірший сценарій для бінарного дерева пошуку\n");
}

void compare_scenarios() {
    printf("\n=== ПОРІВНЯЛЬНИЙ АНАЛІЗ ===\n");

    const int TEST_SIZE = 100;

    printf("Порівнюємо дерева з %d елементів:\n\n", TEST_SIZE);

    // Випадкове дерево
    srand(42); // Фіксований seed для відтворюваності
    Treenode *random_tree = generate_random_tree(TEST_SIZE);
    BalanceInfo random_info = analyze_balance(random_tree);

    // Відсортоване дерево
    Treenode *sorted_tree = generate_sorted_tree(TEST_SIZE);
    BalanceInfo sorted_info = analyze_balance(sorted_tree);

    printf("┌─────────────────────────┬─────────────┬─────────────┐\n");
    printf("│ Характеристика          │ Випадкове   │ Відсортоване│\n");
    printf("├─────────────────────────┼─────────────┼─────────────┤\n");
    printf("│ Кількість вузлів        │ %-11d │ %-11d │\n",
           random_info.size, sorted_info.size);
    printf("│ Висота дерева           │ %-11d │ %-11d │\n",
           random_info.height, sorted_info.height);
    printf("│ Ідеальна висота         │ %-11.1f │ %-11.1f │\n",
           random_info.ideal_height, sorted_info.ideal_height);
    printf("│ Коефіцієнт балансу      │ %-11.3f │ %-11.3f │\n",
           random_info.balance_factor, sorted_info.balance_factor);
    printf("│ Мін. глибина листків    │ %-11d │ %-11d │\n",
           random_info.min_depth, sorted_info.min_depth);
    printf("│ Макс. глибина листків   │ %-11d │ %-11d │\n",
           random_info.max_depth, sorted_info.max_depth);
    printf("└─────────────────────────┴─────────────┴─────────────┘\n");

    printf("\nЕфективність пошуку:\n");
    printf("• Випадкове: в середньому %d порівнянь\n", random_info.height);
    printf("• Відсортоване: в середньому %d порівнянь\n", sorted_info.height);
    printf("• Різниця: у %.1f разів повільніше для відсортованого\n",
           (double)sorted_info.height / random_info.height);

    freetree(random_tree);
    freetree(sorted_tree);
}

void practical_recommendations() {
    printf("\n=== ПРАКТИЧНІ РЕКОМЕНДАЦІЇ ===\n");

    printf("ПРОБЛЕМА:\n");
    printf("Звичайні бінарні дерева пошуку деградують на відсортованих даних\n\n");

    printf("РІШЕННЯ:\n");
    printf("1. Самобалансуючі дерева:\n");
    printf("   • AVL-дерева (строга збалансованість)\n");
    printf("   • Червоно-чорні дерева (послаблена збалансованість)\n");
    printf("   • Splay-дерева (адаптивна збалансованість)\n\n");

    printf("2. Попередня обробка даних:\n");
    printf("   • Перемішування даних перед вставкою\n");
    printf("   • Рекурсивна побудова з середніх елементів\n\n");

    printf("3. Альтернативні структури:\n");
    printf("   • B-дерева для роботи з диском\n");
    printf("   • Хеш-таблиці для O(1) пошуку\n");
    printf("   • Skip Lists як альтернатива\n\n");

    printf("ВИБІР СТРАТЕГІЇ:\n");
    printf("• Якщо дані випадкові → звичайне BST\n");
    printf("• Якщо дані можуть бути відсортовані → самобалансуюче дерево\n");
    printf("• Якщо пріоритет швидкості → хеш-таблиця\n");
    printf("• Якщо потрібен впорядкований обхід → збалансоване дерево\n");
}

int main(void) {
    printf("=== Завдання 14: Аналіз збалансованості бінарних дерев ===\n");

    demonstrate_random_tree();
    demonstrate_sorted_tree();
    compare_scenarios();
    practical_recommendations();


    return 0;
}