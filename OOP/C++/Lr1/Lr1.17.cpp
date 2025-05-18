#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <algorithm>

// Структура для вузла дерева пошуку
typedef struct Tnode {
    char* name;           // Ключ для пошуку
    int value;            // Асоційоване значення
    struct Tnode* left;   // Лівий нащадок
    struct Tnode* right;  // Правий нащадок
} Tnode;

// Структура для вузла з візитором (використовується в Tree-traverser)
typedef struct Treenode {
    char* word;                  // Слово
    struct Treenode* left;       // Лівий нащадок
    struct Treenode* right;      // Правий нащадок
    void (*visit)(struct Treenode*); // Функція для обходу
} Treenode;

// ========== Функції для Binary Search Tree ==========

// Створення нового вузла дерева пошуку
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
    
    return node;
}

// Функція для додавання вузла до дерева пошуку
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
    
    return tree;
}

// Рекурсивна функція пошуку
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

// Ітераційна функція пошуку
Tnode* nvlookup(Tnode* tree, char* name) {
    while (tree != NULL) {
        int cmp = strcmp(name, tree->name);
        if (cmp == 0) {
            return tree; // Знайдено
        } else if (cmp < 0) {
            tree = tree->left; // Ліве піддерево
        } else {
            tree = tree->right; // Праве піддерево
        }
    }
    
    return NULL; // Не знайдено
}

// Функція для звільнення пам'яті дерева пошуку
void freetree(Tnode* tree) {
    if (tree == NULL) {
        return;
    }
    
    freetree(tree->left);
    freetree(tree->right);
    free(tree->name);
    free(tree);
}

// ========== Функції для Tree-traverser ==========

// Створення нового вузла для дерева з візитором
Treenode* newtnode(char* word) {
    Treenode* node = (Treenode*)malloc(sizeof(Treenode));
    if (node == NULL) {
        return NULL;
    }
    
    node->word = strdup(word);
    if (node->word == NULL) {
        free(node);
        return NULL;
    }
    
    node->left = NULL;
    node->right = NULL;
    node->visit = NULL;
    
    return node;
}

// Додавання вузла до дерева з візитором
Treenode* addtnode(Treenode* tree, char* word) {
    // Якщо дерево порожнє, створюємо новий вузол
    if (tree == NULL) {
        return newtnode(word);
    }
    
    // Рекурсивно додаємо вузол у відповідне піддерево
    int cmp = strcmp(word, tree->word);
    if (cmp < 0) {
        tree->left = addtnode(tree->left, word);
    } else if (cmp > 0) {
        tree->right = addtnode(tree->right, word);
    }
    // Дублікати ігноруємо
    
    return tree;
}

// Функції обходу дерева (на основі Tree-traverser)
void preorder(Treenode* tree) {
    if (tree == NULL) {
        return;
    }
    
    if (tree->visit != NULL) {
        tree->visit(tree);
    }
    preorder(tree->left);
    preorder(tree->right);
}

void inorder(Treenode* tree) {
    if (tree == NULL) {
        return;
    }
    
    inorder(tree->left);
    if (tree->visit != NULL) {
        tree->visit(tree);
    }
    inorder(tree->right);
}

void postorder(Treenode* tree) {
    if (tree == NULL) {
        return;
    }
    
    postorder(tree->left);
    postorder(tree->right);
    if (tree->visit != NULL) {
        tree->visit(tree);
    }
}

// Функція для звільнення пам'яті дерева з візитором
void freetnode(Treenode* tree) {
    if (tree == NULL) {
        return;
    }
    
    freetnode(tree->left);
    freetnode(tree->right);
    free(tree->word);
    free(tree);
}

// ========== Тести для Binary Search Tree ==========

// Допоміжні функції для тестування

// Підрахунок кількості вузлів у дереві
int count_nodes(Tnode* tree) {
    if (tree == NULL) {
        return 0;
    }
    
    return 1 + count_nodes(tree->left) + count_nodes(tree->right);
}

// Перевірка властивості бінарного дерева пошуку
bool is_bst(Tnode* tree) {
    if (tree == NULL) {
        return true;
    }
    
    // Перевіряємо лівого нащадка
    if (tree->left != NULL && strcmp(tree->left->name, tree->name) >= 0) {
        return false;
    }
    
    // Перевіряємо правого нащадка
    if (tree->right != NULL && strcmp(tree->right->name, tree->name) <= 0) {
        return false;
    }
    
    // Рекурсивно перевіряємо піддерева
    return is_bst(tree->left) && is_bst(tree->right);
}

// Тест для функції addtree
void test_addtree() {
    std::cout << "Тест функції addtree:" << std::endl;
    
    // Створення нового дерева
    Tnode* tree = NULL;
    
    // Додавання елементів
    tree = addtree(tree, (char*)"banana", 1);
    tree = addtree(tree, (char*)"apple", 2);
    tree = addtree(tree, (char*)"orange", 3);
    tree = addtree(tree, (char*)"grape", 4);
    tree = addtree(tree, (char*)"kiwi", 5);
    
    // Перевірка властивостей
    assert(count_nodes(tree) == 5);
    assert(is_bst(tree));
    
    // Перевірка оновлення значення
    tree = addtree(tree, (char*)"apple", 10);
    assert(count_nodes(tree) == 5); // Кількість вузлів не змінилася
    assert(lookup(tree, (char*)"apple")->value == 10); // Значення оновлено
    
    // Звільнення пам'яті
    freetree(tree);
    
    std::cout << "  Успішно: Додавання елементів працює правильно" << std::endl;
}

// Тест для функції lookup
void test_lookup() {
    std::cout << "Тест функції lookup:" << std::endl;
    
    // Створення тестового дерева
    Tnode* tree = NULL;
    tree = addtree(tree, (char*)"banana", 1);
    tree = addtree(tree, (char*)"apple", 2);
    tree = addtree(tree, (char*)"orange", 3);
    tree = addtree(tree, (char*)"grape", 4);
    tree = addtree(tree, (char*)"kiwi", 5);
    
    // Пошук існуючих елементів
    Tnode* node;
    
    node = lookup(tree, (char*)"apple");
    assert(node != NULL);
    assert(strcmp(node->name, "apple") == 0);
    assert(node->value == 2);
    
    node = lookup(tree, (char*)"kiwi");
    assert(node != NULL);
    assert(strcmp(node->name, "kiwi") == 0);
    assert(node->value == 5);
    
    // Пошук неіснуючого елемента
    node = lookup(tree, (char*)"nonexistent");
    assert(node == NULL);
    
    // Звільнення пам'яті
    freetree(tree);
    
    std::cout << "  Успішно: Пошук елементів працює правильно" << std::endl;
}

// Тест для функції nvlookup
void test_nvlookup() {
    std::cout << "Тест функції nvlookup:" << std::endl;
    
    // Створення тестового дерева
    Tnode* tree = NULL;
    tree = addtree(tree, (char*)"banana", 1);
    tree = addtree(tree, (char*)"apple", 2);
    tree = addtree(tree, (char*)"orange", 3);
    tree = addtree(tree, (char*)"grape", 4);
    tree = addtree(tree, (char*)"kiwi", 5);
    
    // Пошук існуючих елементів
    Tnode* node;
    
    node = nvlookup(tree, (char*)"apple");
    assert(node != NULL);
    assert(strcmp(node->name, "apple") == 0);
    assert(node->value == 2);
    
    node = nvlookup(tree, (char*)"kiwi");
    assert(node != NULL);
    assert(strcmp(node->name, "kiwi") == 0);
    assert(node->value == 5);
    
    // Пошук неіснуючого елемента
    node = nvlookup(tree, (char*)"nonexistent");
    assert(node == NULL);
    
    // Звільнення пам'яті
    freetree(tree);
    
    std::cout << "  Успішно: Ітераційний пошук працює правильно" << std::endl;
}

// Тест для порівняння результатів lookup і nvlookup
void test_lookup_comparison() {
    std::cout << "Тест порівняння lookup і nvlookup:" << std::endl;
    
    // Створення великого тестового дерева
    Tnode* tree = NULL;
    for (int i = 0; i < 100; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        tree = addtree(tree, buffer, i);
    }
    
    // Порівнюємо результати для існуючих елементів
    for (int i = 0; i < 100; i += 10) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        
        Tnode* result1 = lookup(tree, buffer);
        Tnode* result2 = nvlookup(tree, buffer);
        
        assert(result1 != NULL);
        assert(result2 != NULL);
        assert(result1 == result2);
        assert(result1->value == i);
    }
    
    // Порівнюємо результати для неіснуючих елементів
    {
        Tnode* result1 = lookup(tree, (char*)"nonexistent");
        Tnode* result2 = nvlookup(tree, (char*)"nonexistent");
        
        assert(result1 == NULL);
        assert(result2 == NULL);
    }
    
    // Звільнення пам'яті
    freetree(tree);
    
    std::cout << "  Успішно: Обидві функції пошуку дають однакові результати" << std::endl;
}

// ========== Тести для Tree-traverser ==========

// Глобальні змінні для тестування обходу дерева
std::vector<std::string> traversal_result;

// Функція-візитор для тестування
void test_visitor(Treenode* node) {
    if (node != NULL && node->word != NULL) {
        traversal_result.push_back(node->word);
    }
}

// Функція для перевірки, чи масив відсортований
bool is_array_sorted(const std::vector<std::string>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i-1] > arr[i]) {
            return false;
        }
    }
    return true;
}

// Тест для функцій обходу дерева
void test_tree_traversal() {
    std::cout << "Тест функцій обходу дерева:" << std::endl;
    
    // Створення тестового дерева
    Treenode* tree = NULL;
    const char* words[] = {"banana", "apple", "orange", "grape", "kiwi"};
    for (int i = 0; i < 5; i++) {
        tree = addtnode(tree, (char*)words[i]);
    }
    
    // Встановлюємо візитора для всіх вузлів
    std::vector<Treenode*> nodes;
    std::vector<Treenode*> to_process;
    to_process.push_back(tree);
    
    while (!to_process.empty()) {
        Treenode* node = to_process.back();
        to_process.pop_back();
        
        if (node != NULL) {
            nodes.push_back(node);
            to_process.push_back(node->left);
            to_process.push_back(node->right);
        }
    }
    
    for (Treenode* node : nodes) {
        node->visit = test_visitor;
    }
    
    // Тестування preorder обходу
    traversal_result.clear();
    preorder(tree);
    assert(traversal_result.size() == 5);
    // У pre-order першим буде корінь
    assert(traversal_result[0] == "banana");
    
    // Тестування inorder обходу
    traversal_result.clear();
    inorder(tree);
    assert(traversal_result.size() == 5);
    assert(is_array_sorted(traversal_result));
    // У відсортованому масиві "apple" буде першим
    assert(traversal_result[0] == "apple");
    
    // Тестування postorder обходу
    traversal_result.clear();
    postorder(tree);
    assert(traversal_result.size() == 5);
    // У post-order корінь буде останнім
    assert(traversal_result[traversal_result.size() - 1] == "banana");
    
    // Звільнення пам'яті
    freetnode(tree);
    
    std::cout << "  Успішно: Функції обходу дерева працюють правильно" << std::endl;
}

// Запуск всіх тестів
void run_all_tests() {
    std::cout << "Запуск всіх тестів для функцій роботи з деревами\n" << std::endl;
    
    // Тести для Binary Search Tree
    test_addtree();
    test_lookup();
    test_nvlookup();
    test_lookup_comparison();
    
    // Тести для Tree-traverser
    test_tree_traversal();
    
    std::cout << "\nВсі тести успішно пройдені!" << std::endl;
}

// Тест для перевірки ефективності сортування за допомогою дерева
void test_tree_sorting() {
    std::cout << "\nТест сортування за допомогою бінарного дерева:" << std::endl;
    
    // Створення невідсортованого масиву
    std::vector<std::string> words = {
        "zebra", "monkey", "elephant", "tiger", "lion", "giraffe", 
        "hippo", "rhino", "gazelle", "cheetah"
    };
    
    std::cout << "Початковий масив:" << std::endl;
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    
    // Створення дерева та додавання слів
    Treenode* tree = NULL;
    for (const auto& word : words) {
        tree = addtnode(tree, (char*)word.c_str());
    }
    
    // Встановлення візитора
    std::vector<Treenode*> nodes;
    std::vector<Treenode*> to_process;
    to_process.push_back(tree);
    
    while (!to_process.empty()) {
        Treenode* node = to_process.back();
        to_process.pop_back();
        
        if (node != NULL) {
            nodes.push_back(node);
            to_process.push_back(node->left);
            to_process.push_back(node->right);
        }
    }
    
    for (Treenode* node : nodes) {
        node->visit = test_visitor;
    }
    
    // Сортування за допомогою inorder обходу
    traversal_result.clear();
    inorder(tree);
    
    std::cout << "Відсортований масив:" << std::endl;
    for (const auto& word : traversal_result) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    
    // Перевірка сортування
    assert(is_array_sorted(traversal_result));
    
    // Звільнення пам'яті
    freetnode(tree);
    
    std::cout << "  Успішно: Сортування за допомогою дерева працює правильно" << std::endl;
}

int main() {
    run_all_tests();
    test_tree_sorting();
    
    return 0;
}