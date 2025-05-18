#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

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

// Функція для симетричного обходу дерева (in-order traversal)
// Додає елементи до вектора в відсортованому порядку
void inorder_traversal(Tnode* tree, std::vector<std::pair<std::string, int>>& result) {
    if (tree == NULL) {
        return;
    }
    
    // Спочатку обходимо ліве піддерево
    inorder_traversal(tree->left, result);
    
    // Додаємо поточний вузол
    result.push_back(std::make_pair(std::string(tree->name), tree->value));
    
    // Потім обходимо праве піддерево
    inorder_traversal(tree->right, result);
}

// Функція сортування з використанням бінарного дерева
std::vector<std::pair<std::string, int>> tree_sort(const std::vector<std::pair<std::string, int>>& data) {
    // Створюємо дерево і додаємо всі елементи
    Tnode* tree = NULL;
    for (const auto& item : data) {
        tree = addtree(tree, (char*)item.first.c_str(), item.second);
    }
    
    // Виконуємо симетричний обхід для отримання відсортованого результату
    std::vector<std::pair<std::string, int>> result;
    inorder_traversal(tree, result);
    
    // Звільняємо пам'ять
    freetree(tree);
    
    return result;
}

// Функція для генерації випадкових даних
std::vector<std::pair<std::string, int>> generate_random_data(int count) {
    std::vector<std::pair<std::string, int>> data;
    
    for (int i = 0; i < count; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", rand() % 10000); // Випадкові імена
        data.push_back(std::make_pair(buffer, rand() % 1000)); // Випадкові значення
    }
    
    return data;
}

// Функція для генерації даних у зворотному порядку
std::vector<std::pair<std::string, int>> generate_reverse_data(int count) {
    std::vector<std::pair<std::string, int>> data;
    
    for (int i = count - 1; i >= 0; i--) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        data.push_back(std::make_pair(buffer, i));
    }
    
    return data;
}

// Функція для перевірки, чи відсортовано масив
bool is_sorted(const std::vector<std::pair<std::string, int>>& data) {
    for (size_t i = 1; i < data.size(); i++) {
        if (data[i-1].first > data[i].first) {
            return false;
        }
    }
    return true;
}

// Порівняння різних методів сортування
void compare_sorting_methods() {
    const int DATA_SIZE = 10000;
    
    std::cout << "Генерація " << DATA_SIZE << " випадкових елементів..." << std::endl;
    auto random_data = generate_random_data(DATA_SIZE);
    
    // Копії даних для різних методів сортування
    auto data_copy1 = random_data;
    auto data_copy2 = random_data;
    
    // Сортування з використанням дерева
    std::cout << "Сортування з використанням бінарного дерева..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto sorted_data_tree = tree_sort(random_data);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto tree_sort_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Перевірка коректності сортування
    bool tree_sort_correct = is_sorted(sorted_data_tree);
    
    // Сортування з використанням std::sort
    std::cout << "Сортування з використанням std::sort..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    std::sort(data_copy1.begin(), data_copy1.end(), 
             [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                 return a.first < b.first;
             });
    end_time = std::chrono::high_resolution_clock::now();
    auto std_sort_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Сортування злиттям (через std::stable_sort)
    std::cout << "Сортування з використанням std::stable_sort (сортування злиттям)..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    std::stable_sort(data_copy2.begin(), data_copy2.end(), 
                    [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                        return a.first < b.first;
                    });
    end_time = std::chrono::high_resolution_clock::now();
    auto merge_sort_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Виведення результатів
    std::cout << "\nРезультати порівняння методів сортування для " << DATA_SIZE << " елементів:" << std::endl;
    std::cout << "1. Сортування з використанням бінарного дерева: " << tree_sort_duration << " мс";
    std::cout << (tree_sort_correct ? " (коректно)" : " (некоректно)") << std::endl;
    std::cout << "2. Стандартне швидке сортування (std::sort): " << std_sort_duration << " мс" << std::endl;
    std::cout << "3. Сортування злиттям (std::stable_sort): " << merge_sort_duration << " мс" << std::endl;
    
    // Аналіз швидкодії для неоптимальних даних
    std::cout << "\nАналіз швидкодії для неоптимальних даних (зворотний порядок):" << std::endl;
    auto reverse_data = generate_reverse_data(DATA_SIZE);
    
    // Сортування з використанням дерева
    start_time = std::chrono::high_resolution_clock::now();
    auto sorted_reverse_tree = tree_sort(reverse_data);
    end_time = std::chrono::high_resolution_clock::now();
    auto tree_reverse_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Сортування з використанням std::sort
    auto reverse_copy = reverse_data;
    start_time = std::chrono::high_resolution_clock::now();
    std::sort(reverse_copy.begin(), reverse_copy.end(), 
             [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                 return a.first < b.first;
             });
    end_time = std::chrono::high_resolution_clock::now();
    auto std_reverse_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    std::cout << "1. Сортування з використанням бінарного дерева: " << tree_reverse_duration << " мс" << std::endl;
    std::cout << "2. Стандартне швидке сортування (std::sort): " << std_reverse_duration << " мс" << std::endl;
}

// Аналіз часової складності
void analyze_time_complexity() {
    std::cout << "\nАналіз часової складності дерев'яного сортування:" << std::endl;
    
    std::cout << "Порядок складності дерев'яного сортування:" << std::endl;
    std::cout << "- Побудова дерева: O(n log n) для випадкового порядку даних" << std::endl;
    std::cout << "- Побудова дерева: O(n²) для відсортованих даних (виродження в список)" << std::endl;
    std::cout << "- Симетричний обхід: O(n)" << std::endl;
    std::cout << "- Загальна складність: O(n log n) для випадкового порядку даних" << std::endl;
    std::cout << "- Загальна складність: O(n²) для відсортованих даних" << std::endl;
    
    std::cout << "\nУмови, при яких сортування дерева працює погано:" << std::endl;
    std::cout << "1. Дані вже відсортовані або майже відсортовані (дерево вироджується в список)" << std::endl;
    std::cout << "2. Дані в зворотному порядку (дерево також вироджується в список)" << std::endl;
    std::cout << "3. Велика кількість дублікатів ключів (ефективність залежить від реалізації)" << std::endl;
    
    std::cout << "\nХарактеристики в порівнянні з іншими алгоритмами:" << std::endl;
    std::cout << "- Швидке сортування (std::sort): краще для загального випадку з часовою" << std::endl;
    std::cout << "  складністю O(n log n) і просторовою складністю O(log n)" << std::endl;
    std::cout << "- Сортування злиттям: стабільне сортування з часовою складністю O(n log n)" << std::endl;
    std::cout << "  і просторовою складністю O(n)" << std::endl;
    std::cout << "- Сортування деревом: простота реалізації, стабільність, але потенційно" << std::endl;
    std::cout << "  більше використання пам'яті (O(n) для зберігання дерева)" << std::endl;
    
    std::cout << "\nОсобливості дерев'яного сортування:" << std::endl;
    std::cout << "- Просте і природне відображення процесу сортування" << std::endl;
    std::cout << "- Можливість використання для інших задач (пошук, перевірка, інше)" << std::endl;
    std::cout << "- Отримання доступу до даних в відсортованому порядку без зберігання" << std::endl;
    std::cout << "  всього відсортованого масиву" << std::endl;
    std::cout << "- Додавання нових елементів не вимагає повного пересортування" << std::endl;
}

int main() {
    // Ініціалізація генератора випадкових чисел
    srand(time(NULL));
    
    // Демонстрація сортування простого масиву
    std::cout << "Демонстрація сортування з симетричним обходом дерева:" << std::endl;
    std::vector<std::pair<std::string, int>> data = {
        {"banana", 2},
        {"apple", 5},
        {"orange", 3},
        {"grape", 1},
        {"kiwi", 4}
    };
    
    std::cout << "Початкові дані:" << std::endl;
    for (const auto& item : data) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
    
    auto sorted_data = tree_sort(data);
    
    std::cout << "\nВідсортовані дані:" << std::endl;
    for (const auto& item : sorted_data) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
    
    // Порівняння методів сортування
    std::cout << "\n======================================" << std::endl;
    compare_sorting_methods();
    
    // Аналіз часової складності
    analyze_time_complexity();
    
    return 0;
}