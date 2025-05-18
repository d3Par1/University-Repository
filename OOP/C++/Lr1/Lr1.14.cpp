#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>

// Структура для вузла дерева
typedef struct Tnode {
    char* name;           // Ключ для пошуку
    int value;            // Асоційоване значення
    struct Tnode* left;   // Лівий нащадок
    struct Tnode* right;  // Правий нащадок
    int height;           // Висота піддерева (для перевірки збалансованості)
} Tnode;

// Створення нового вузла дерева
Tnode* newnode(char* name, int value) {
    Tnode* node = (Tnode*)malloc(sizeof(Tnode));
    if (node == NULL) {
        return NULL;
    }
    
    node->name = strdup(name);
    if (node->name == NULL) {
        free(node);
        return NULL;
    }
    
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // Новий вузол має висоту 1
    
    return node;
}

// Функція для обчислення максимуму
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Функція для оновлення висоти вузла
void update_height(Tnode* node) {
    if (node == NULL) {
        return;
    }
    
    int left_height = (node->left != NULL) ? node->left->height : 0;
    int right_height = (node->right != NULL) ? node->right->height : 0;
    
    node->height = 1 + max(left_height, right_height);
}

// Функція для додавання вузла до дерева
Tnode* addtree(Tnode* tree, char* name, int value) {
    // Якщо дерево порожнє, створюємо новий вузол
    if (tree == NULL) {
        return newnode(name, value);
    }
    
    // Рекурсивно додаємо вузол у відповідне піддерево
    int cmp = strcmp(name, tree->name);
    if (cmp < 0) {
        tree->left = addtree(tree->left, name, value);
    } else if (cmp > 0) {
        tree->right = addtree(tree->right, name, value);
    } else {
        // Елемент вже існує, оновлюємо значення
        tree->value = value;
    }
    
    // Оновлюємо висоту вузла
    update_height(tree);
    
    return tree;
}

// Функція для пошуку вузла за ім'ям
Tnode* lookup(Tnode* tree, char* name) {
    if (tree == NULL) {
        return NULL;
    }
    
    int cmp = strcmp(name, tree->name);
    if (cmp == 0) {
        return tree;
    } else if (cmp < 0) {
        return lookup(tree->left, name);
    } else {
        return lookup(tree->right, name);
    }
}

// Функція для звільнення пам'яті дерева
void freetree(Tnode* tree) {
    if (tree == NULL) {
        return;
    }
    
    freetree(tree->left);
    freetree(tree->right);
    free(tree->name);
    free(tree);
}

// Функція для обчислення коефіцієнта збалансованості (balance factor)
int get_balance_factor(Tnode* node) {
    if (node == NULL) {
        return 0;
    }
    
    int left_height = (node->left != NULL) ? node->left->height : 0;
    int right_height = (node->right != NULL) ? node->right->height : 0;
    
    return left_height - right_height;
}

// Функція для перевірки збалансованості дерева
bool is_balanced(Tnode* tree) {
    if (tree == NULL) {
        return true;
    }
    
    // Перевіряємо баланс поточного вузла
    int balance = get_balance_factor(tree);
    if (balance < -1 || balance > 1) {
        return false;
    }
    
    // Рекурсивно перевіряємо піддерева
    return is_balanced(tree->left) && is_balanced(tree->right);
}

// Функція для обчислення висоти дерева
int get_tree_height(Tnode* tree) {
    if (tree == NULL) {
        return 0;
    }
    
    return tree->height;
}

// Функція для обчислення кількості вузлів у дереві
int count_nodes(Tnode* tree) {
    if (tree == NULL) {
        return 0;
    }
    
    return 1 + count_nodes(tree->left) + count_nodes(tree->right);
}

// Функція для виведення дерева (in-order traversal)
void print_tree(Tnode* tree, int level = 0) {
    if (tree == NULL) {
        return;
    }
    
    print_tree(tree->right, level + 1);
    
    for (int i = 0; i < level; i++) {
        std::cout << "    ";
    }
    std::cout << tree->name << " (" << tree->value << ")" << std::endl;
    
    print_tree(tree->left, level + 1);
}

// Функція для аналізу збалансованості дерева
void analyze_tree_balance(Tnode* tree) {
    int node_count = count_nodes(tree);
    int height = get_tree_height(tree);
    double optimal_height = log2(node_count + 1);
    
    std::cout << "Аналіз дерева:" << std::endl;
    std::cout << "  Кількість вузлів: " << node_count << std::endl;
    std::cout << "  Висота дерева: " << height << std::endl;
    std::cout << "  Оптимальна висота для збалансованого дерева: ~" 
              << optimal_height << std::endl;
    std::cout << "  Коефіцієнт ефективності: " 
              << optimal_height / height << " (ближче до 1 - краще)" << std::endl;
    
    if (is_balanced(tree)) {
        std::cout << "  Дерево є збалансованим (різниця висот підерев не більше 1)" << std::endl;
    } else {
        std::cout << "  Дерево не є збалансованим" << std::endl;
    }
}

// Генерація випадкових даних
std::vector<std::pair<std::string, int>> generate_random_data(int count) {
    std::vector<std::pair<std::string, int>> data;
    
    for (int i = 0; i < count; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        data.push_back(std::make_pair(buffer, i * 10));
    }
    
    // Перемішуємо дані для випадкового порядку
    std::random_shuffle(data.begin(), data.end());
    
    return data;
}

// Генерація відсортованих даних
std::vector<std::pair<std::string, int>> generate_sorted_data(int count) {
    std::vector<std::pair<std::string, int>> data;
    
    for (int i = 0; i < count; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        data.push_back(std::make_pair(buffer, i * 10));
    }
    
    return data;
}

int main() {
    // Ініціалізуємо генератор випадкових чисел
    srand(time(NULL));
    
    const int DATA_SIZE = 100; // Розмір набору даних
    
    std::cout << "=== Варіант 1: Елементи надходять у випадковому порядку ===" << std::endl;
    
    // Створюємо набір випадкових даних
    std::vector<std::pair<std::string, int>> random_data = generate_random_data(DATA_SIZE);
    
    // Будуємо дерево з випадкових даних
    Tnode* random_tree = NULL;
    for (const auto& item : random_data) {
        random_tree = addtree(random_tree, (char*)item.first.c_str(), item.second);
    }
    
    // Аналізуємо збалансованість дерева
    analyze_tree_balance(random_tree);
    
    // Виводимо структуру дерева (для невеликих дерев)
    if (DATA_SIZE <= 10) {
        std::cout << "\nСтруктура дерева:" << std::endl;
        print_tree(random_tree);
    }
    
    // Звільняємо пам'ять
    freetree(random_tree);
    
    std::cout << "\n=== Варіант 2: Елементи надходять у відсортованому вигляді ===" << std::endl;
    
    // Створюємо набір відсортованих даних
    std::vector<std::pair<std::string, int>> sorted_data = generate_sorted_data(DATA_SIZE);
    
    // Будуємо дерево з відсортованих даних
    Tnode* sorted_tree = NULL;
    for (const auto& item : sorted_data) {
        sorted_tree = addtree(sorted_tree, (char*)item.first.c_str(), item.second);
    }
    
    // Аналізуємо збалансованість дерева
    analyze_tree_balance(sorted_tree);
    
    // Виводимо структуру дерева (для невеликих дерев)
    if (DATA_SIZE <= 10) {
        std::cout << "\nСтруктура дерева:" << std::endl;
        print_tree(sorted_tree);
    }
    
    // Звільняємо пам'ять
    freetree(sorted_tree);
    
    std::cout << "\n=== Порівняння та висновки ===" << std::endl;
    std::cout << "1. Випадковий порядок:" << std::endl;
    std::cout << "   - При випадковому порядку надходження даних дерево з високою" << std::endl;
    std::cout << "     імовірністю буде більш-менш збалансованим." << std::endl;
    std::cout << "   - Це пояснюється тим, що випадковий порядок дозволяє елементам" << std::endl;
    std::cout << "     рівномірно розподілятися між лівим і правим піддеревами." << std::endl;
    
    std::cout << "\n2. Відсортований порядок:" << std::endl;
    std::cout << "   - При відсортованому порядку надходження даних дерево" << std::endl;
    std::cout << "     виродиться в фактично однобічний зв'язаний список." << std::endl;
    std::cout << "   - Кожен новий елемент буде додаватися до правого піддерева," << std::endl;
    std::cout << "     що призведе до глибокої структури з максимальною висотою O(n)." << std::endl;
    std::cout << "   - Це підтверджує твердження, що при відсортованому порядку" << std::endl;
    std::cout << "     дерево має проблеми швидкодії, властиві спискам." << std::endl;
    
    std::cout << "\nЗагальний висновок:" << std::endl;
    std::cout << "   Порядок додавання елементів критично впливає на збалансованість" << std::endl;
    std::cout << "   та ефективність двійкового дерева пошуку. Для гарантованого" << std::endl;
    std::cout << "   збереження збалансованості потрібно використовувати спеціальні" << std::endl;
    std::cout << "   самобалансуючі дерева, такі як AVL або Red-Black дерева, які" << std::endl;
    std::cout << "   автоматично перебудовуються для збереження балансу." << std::endl;
    
    return 0;
}