#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <chrono>      // ДОДАНО: потрібно для std::chrono
#include <vector>
#include <random>
#include <algorithm>
// Структура для вузла дерева
typedef struct Tnode {
    char* name;           // Ключ для пошуку
    int value;            // Асоційоване значення
    struct Tnode* left;   // Лівий нащадок
    struct Tnode* right;  // Правий нащадок
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
    
    return node;
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
    
    return tree;
}

// Рекурсивна функція пошуку (lookup)
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

// Ітераційна функція пошуку (nvlookup)
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

// Функція для генерації випадкового рядка
std::string generate_random_string(int length) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    std::string result;
    result.reserve(length);
    
    for (int i = 0; i < length; i++) {
        result += charset[rand() % (sizeof(charset) - 1)];
    }
    
    return result;
}

// Функція для підготовки великого тестового дерева
Tnode* prepare_large_tree(int size) {
    Tnode* tree = NULL;
    
    for (int i = 0; i < size; i++) {
        std::string key = "key" + std::to_string(i);
        tree = addtree(tree, (char*)key.c_str(), i);
    }
    
    return tree;
}

// Функція для підготовки набору пошукових ключів
std::vector<std::string> prepare_search_keys(int count, int max_key, double hit_ratio) {
    std::vector<std::string> keys;
    
    int hit_count = static_cast<int>(count * hit_ratio);
    int miss_count = count - hit_count;
    
    // Додаємо ключі, які існують у дереві
    for (int i = 0; i < hit_count; i++) {
        int key_num = rand() % max_key;
        keys.push_back("key" + std::to_string(key_num));
    }
    
    // Додаємо ключі, яких немає в дереві
    for (int i = 0; i < miss_count; i++) {
        keys.push_back("nonexistent" + std::to_string(i));
    }
    
    // Перемішуємо ключі
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(keys.begin(), keys.end(), g);
    
    return keys;
}

// Функція для порівняння швидкодії рекурсивної та ітераційної функцій пошуку
void compare_lookup_performance() {
    const int TREE_SIZE = 10000;
    const int SEARCH_COUNT = 100000;
    const double HIT_RATIO = 0.7; // 70% запитів повинні знаходити елементи
    
    // Підготовка тестового дерева
    std::cout << "Підготовка тестового дерева з " << TREE_SIZE << " елементів..." << std::endl;
    Tnode* tree = prepare_large_tree(TREE_SIZE);
    
    // Підготовка набору пошукових ключів
    std::cout << "Підготовка " << SEARCH_COUNT << " пошукових запитів..." << std::endl;
    std::vector<std::string> search_keys = prepare_search_keys(SEARCH_COUNT, TREE_SIZE, HIT_RATIO);
    
    // Тестування рекурсивної функції lookup
    std::cout << "Тестування рекурсивної функції lookup..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    int found_count_recursive = 0;
    for (const auto& key : search_keys) {
        Tnode* result = lookup(tree, (char*)key.c_str());
        if (result != NULL) {
            found_count_recursive++;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto recursive_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Тестування ітераційної функції nvlookup
    std::cout << "Тестування ітераційної функції nvlookup..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    
    int found_count_iterative = 0;
    for (const auto& key : search_keys) {
        Tnode* result = nvlookup(tree, (char*)key.c_str());
        if (result != NULL) {
            found_count_iterative++;
        }
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto iterative_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Виведення результатів
    std::cout << "\nРезультати порівняння:" << std::endl;
    std::cout << "  Рекурсивна функція lookup:" << std::endl;
    std::cout << "    Час виконання: " << recursive_duration << " мс" << std::endl;
    std::cout << "    Знайдено елементів: " << found_count_recursive << std::endl;
    
    std::cout << "  Ітераційна функція nvlookup:" << std::endl;
    std::cout << "    Час виконання: " << iterative_duration << " мс" << std::endl;
    std::cout << "    Знайдено елементів: " << found_count_iterative << std::endl;
    
    double speedup = (double)recursive_duration / iterative_duration;
    std::cout << "\n  Прискорення ітераційної функції: " << speedup << "x" << std::endl;
    
    // Звільнення пам'яті
    freetree(tree);
}

// Функція для демонстрації різниці між рекурсією та ітерацією на великому дереві
void demonstrate_stack_overflow() {
    const int DEEP_TREE_SIZE = 10000;
    
    std::cout << "\nДемонстрація проблеми переповнення стеку при рекурсії:" << std::endl;
    std::cout << "Підготовка глибокого дерева з " << DEEP_TREE_SIZE << " елементів..." << std::endl;
    
    // Створюємо дерево, яке виродиться в правий список
    Tnode* tree = NULL;
    for (int i = 0; i < DEEP_TREE_SIZE; i++) {
        std::string key = std::to_string(i);
        tree = addtree(tree, (char*)key.c_str(), i);
    }
    
    std::cout << "Пошук останнього елемента (" << (DEEP_TREE_SIZE - 1) << ") в глибокому дереві:" << std::endl;
    
    // Ітераційний пошук
    std::cout << "  Ітераційний пошук... ";
    try {
        std::string last_key = std::to_string(DEEP_TREE_SIZE - 1);
        Tnode* result = nvlookup(tree, (char*)last_key.c_str());
        if (result != NULL) {
            std::cout << "Успішно знайдено" << std::endl;
        } else {
            std::cout << "Елемент не знайдено (це не повинно статися)" << std::endl;
        }
    } catch (...) {
        std::cout << "Сталася помилка!" << std::endl;
    }
    
    // Рекурсивний пошук (може викликати переповнення стеку на дуже глибоких деревах)
    std::cout << "  Рекурсивний пошук... ";
    try {
        std::string last_key = std::to_string(DEEP_TREE_SIZE - 1);
        Tnode* result = lookup(tree, (char*)last_key.c_str());
        if (result != NULL) {
            std::cout << "Успішно знайдено" << std::endl;
        } else {
            std::cout << "Елемент не знайдено (це не повинно статися)" << std::endl;
        }
    } catch (...) {
        std::cout << "Сталася помилка (можливо, переповнення стеку)!" << std::endl;
    }
    
    // Звільнення пам'яті
    freetree(tree);
}

int main() {
    // Ініціалізація генератора випадкових чисел
    srand(time(NULL));
    
    compare_lookup_performance();
    
    // Коментуємо цю частину, оскільки вона може призвести до аварійного завершення програми
    // demonstrate_stack_overflow();
    
    std::cout << "\nАналіз відмінностей між рекурсивною та ітераційною формами:" << std::endl;
    std::cout << "1. Рекурсивна форма (lookup):" << std::endl;
    std::cout << "   + Проста і природна структура коду, що відповідає структурі дерева" << std::endl;
    std::cout << "   + Легше зрозуміти і реалізувати для початківців" << std::endl;
    std::cout << "   - Використовує стек викликів, що вимагає додаткової пам'яті O(h), де h - висота дерева" << std::endl;
    std::cout << "   - Ризик переповнення стеку при роботі з глибокими деревами" << std::endl;
    std::cout << "   - Дещо повільніша через накладні витрати на створення стекових кадрів" << std::endl;
    
    std::cout << "\n2. Ітераційна форма (nvlookup):" << std::endl;
    std::cout << "   + Ефективніша за використанням пам'яті (O(1) додаткової пам'яті)" << std::endl;
    std::cout << "   + Швидша за рахунок уникнення накладних витрат на рекурсивні виклики" << std::endl;
    std::cout << "   + Не має ризику переповнення стеку навіть для дуже глибоких дерев" << std::endl;
    std::cout << "   - Менш інтуїтивно зрозуміла структура коду" << std::endl;
    std::cout << "   - Може бути складнішою для реалізації в деяких алгоритмах" << std::endl;
    
    std::cout << "\nВисновок: Ітераційний варіант є більш ефективним для великих дерев," << std::endl;
    std::cout << "але рекурсивний варіант може бути кращим вибором для підтримки коду" << std::endl;
    std::cout << "та для невеликих дерев, де швидкодія не критична." << std::endl;
    
    return 0;
}