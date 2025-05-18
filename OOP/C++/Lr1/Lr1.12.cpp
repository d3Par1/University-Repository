#include <iostream>
#include <cstring>
#include <cstdlib>

// ======= Частина 1: Нетипізований список на C =======

// Нетипізована структура Node для C
typedef struct TypeNode {
    void* data;           // Вказівник на будь-які дані
    struct TypeNode* next; // Вказівник на наступний елемент
} TypeNode;

// Структура для управління списком
typedef struct Type_List {
    TypeNode* head;       // Голова списку
    int size;             // Розмір списку
    
    // Додаткове поле: функція для звільнення пам'яті даних
    void (*free_data)(void*);
} Type_List;

// Ініціалізація нетипізованого списку
void init_list(Type_List* list, void (*free_func)(void*)) {
    list->head = NULL;
    list->size = 0;
    list->free_data = free_func;
}

// Додавання елементу до нетипізованого списку
int add_to_list(Type_List* list, void* data) {
    TypeNode* new_node = (TypeNode*)malloc(sizeof(TypeNode));
    if (new_node == NULL) {
        return 0; // Помилка виділення пам'яті
    }
    
    new_node->data = data;
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
    
    return 1; // Успіх
}

// Видалення елементу з нетипізованого списку за заданим вказівником на дані
int remove_from_list(Type_List* list, void* data, int (*compare)(void*, void*)) {
    TypeNode* current = list->head;
    TypeNode* prev = NULL;
    
    while (current != NULL) {
        if (compare(current->data, data) == 0) {
            // Знайдено елемент для видалення
            if (prev == NULL) {
                // Видалення першого елементу
                list->head = current->next;
            } else {
                // Видалення з середини або кінця
                prev->next = current->next;
            }
            
            // Звільнення пам'яті
            if (list->free_data != NULL) {
                list->free_data(current->data);
            }
            free(current);
            list->size--;
            
            return 1; // Успіх
        }
        
        prev = current;
        current = current->next;
    }
    
    return 0; // Елемент не знайдено
}

// Звільнення всього нетипізованого списку
void free_list(Type_List* list) {
    TypeNode* current = list->head;
    TypeNode* next;
    
    while (current != NULL) {
        next = current->next;
        
        // Звільнення даних, якщо є функція для цього
        if (list->free_data != NULL) {
            list->free_data(current->data);
        }
        
        free(current);
        current = next;
    }
    
    list->head = NULL;
    list->size = 0;
}

// Обробка елементів нетипізованого списку
void process_list(Type_List* list, void (*processor)(void*)) {
    TypeNode* current = list->head;
    
    while (current != NULL) {
        processor(current->data);
        current = current->next;
    }
}

// ======= Частина 2: Шаблонний список на C++ =======

// Шаблонний клас вузла списку для C++
template <typename T>
class Node {
public:
    T data;
    Node<T>* next;
    
    Node(const T& value) : data(value), next(nullptr) {}
};

// Шаблонний клас списку для C++
template <typename T>
class List {
private:
    Node<T>* head;
    int size;
    
public:
    List() : head(nullptr), size(0) {}
    
    ~List() {
        clear();
    }
    
    // Додавання елементу до початку списку
    void push_front(const T& value) {
        Node<T>* new_node = new Node<T>(value);
        new_node->next = head;
        head = new_node;
        size++;
    }
    
    // Видалення елементу зі списку
    bool remove(const T& value) {
        Node<T>* current = head;
        Node<T>* prev = nullptr;
        
        while (current != nullptr) {
            if (current->data == value) {
                if (prev == nullptr) {
                    // Видалення першого елементу
                    head = current->next;
                } else {
                    // Видалення з середини або кінця
                    prev->next = current->next;
                }
                
                delete current;
                size--;
                return true;
            }
            
            prev = current;
            current = current->next;
        }
        
        return false; // Елемент не знайдено
    }
    
    // Очищення всього списку
    void clear() {
        Node<T>* current = head;
        Node<T>* next;
        
        while (current != nullptr) {
            next = current->next;
            delete current;
            current = next;
        }
        
        head = nullptr;
        size = 0;
    }
    
    // Перевірка, чи є список порожнім
    bool empty() const {
        return head == nullptr;
    }
    
    // Отримання розміру списку
    int get_size() const {
        return size;
    }
    
    // Обробка всіх елементів списку
    void for_each(void (*processor)(const T&)) const {
        Node<T>* current = head;
        
        while (current != nullptr) {
            processor(current->data);
            current = current->next;
        }
    }
};

// ======= Приклад використання обох реалізацій =======

// Структура для демонстрації нетипізованого списку
typedef struct Person {
    char* name;
    int age;
} Person;

// Функція для звільнення пам'яті Person
void free_person(void* data) {
    Person* person = (Person*)data;
    free(person->name);
    free(person);
}

// Функція для виведення Person
void print_person(void* data) {
    Person* person = (Person*)data;
    std::cout << "Ім'я: " << person->name << ", Вік: " << person->age << std::endl;
}

// Функція для порівняння Person за ім'ям
int compare_person_name(void* data1, void* data2) {
    Person* person1 = (Person*)data1;
    Person* person2 = (Person*)data2;
    return strcmp(person1->name, person2->name);
}

// Функція для виведення int
void print_int(const int& value) {
    std::cout << value << " ";
}

int main() {
    std::cout << "===== Демонстрація нетипізованого списку на C =====" << std::endl;
    
    // Створення та ініціалізація нетипізованого списку
    Type_List person_list;
    init_list(&person_list, free_person);
    
    // Створення та додавання елементів
    Person* p1 = (Person*)malloc(sizeof(Person));
    p1->name = strdup("Іван");
    p1->age = 25;
    add_to_list(&person_list, p1);
    
    Person* p2 = (Person*)malloc(sizeof(Person));
    p2->name = strdup("Марія");
    p2->age = 30;
    add_to_list(&person_list, p2);
    
    Person* p3 = (Person*)malloc(sizeof(Person));
    p3->name = strdup("Петро");
    p3->age = 22;
    add_to_list(&person_list, p3);
    
    // Виведення елементів
    std::cout << "Список осіб:" << std::endl;
    process_list(&person_list, print_person);
    
    // Видалення елементу
    Person search_person;
    search_person.name = (char*)"Марія";
    remove_from_list(&person_list, &search_person, compare_person_name);
    
    std::cout << "\nСписок після видалення 'Марія':" << std::endl;
    process_list(&person_list, print_person);
    
    // Звільнення списку
    free_list(&person_list);
    
    std::cout << "\n\n===== Демонстрація шаблонного списку на C++ =====" << std::endl;
    
    // Створення шаблонного списку цілих чисел
    List<int> int_list;
    
    // Додавання елементів
    for (int i = 1; i <= 5; i++) {
        int_list.push_front(i * 10);
    }
    
    // Виведення елементів
    std::cout << "Список цілих чисел:" << std::endl;
    int_list.for_each(print_int);
    std::cout << std::endl;
    
    // Видалення елементу
    int_list.remove(30);
    
    std::cout << "Список після видалення 30:" << std::endl;
    int_list.for_each(print_int);
    std::cout << std::endl;
    
    // Очищення списку автоматично при виході зі scope
    
    std::cout << "\n===== Порівняння підходів =====" << std::endl;
    std::cout << "Сильні сторони нетипізованого списку на C:" << std::endl;
    std::cout << "1. Простота реалізації без знання шаблонів" << std::endl;
    std::cout << "2. Можливість зберігати елементи різних типів в одному списку" << std::endl;
    std::cout << "3. Сумісність з кодом на чистому C" << std::endl;
    
    std::cout << "\nСлабкі сторони нетипізованого списку на C:" << std::endl;
    std::cout << "1. Відсутність перевірки типів на етапі компіляції" << std::endl;
    std::cout << "2. Необхідність явного приведення типів і роботи з вказівниками" << std::endl;
    std::cout << "3. Вища вірогідність помилок, пов'язаних з управлінням пам'яттю" << std::endl;
    std::cout << "4. Складніше використання для клієнтського коду" << std::endl;
    
    std::cout << "\nСильні сторони шаблонного списку на C++:" << std::endl;
    std::cout << "1. Перевірка типів на етапі компіляції" << std::endl;
    std::cout << "2. Безпечніша та зрозуміліша робота з типами (без void*)" << std::endl;
    std::cout << "3. Автоматичне керування ресурсами (деструктори)" << std::endl;
    std::cout << "4. Зручніший інтерфейс для використання" << std::endl;
    
    std::cout << "\nСлабкі сторони шаблонного списку на C++:" << std::endl;
    std::cout << "1. Складніша реалізація, потребує знання шаблонів" << std::endl;
    std::cout << "2. Окремий екземпляр коду генерується для кожного типу" << std::endl;
    std::cout << "3. Несумісність з чистим кодом на C" << std::endl;
    
    return 0;
}