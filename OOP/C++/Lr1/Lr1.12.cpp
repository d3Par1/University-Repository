#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct ListNode ListNode;

struct ListNode {
    void *data;          // Універсальний вказівник на дані
    ListNode *next;
};

typedef struct {
    ListNode *head;
    int count;
    size_t data_size;    // Розмір одного елемента даних

    // Функції для роботи з даними
    int (*compare)(const void *a, const void *b);
    void (*print)(const void *data);
    void (*destroy)(void *data);
} GenericList;

// Створення нового списку
GenericList* create_list(size_t data_size,
                        int (*compare_func)(const void *a, const void *b),
                        void (*print_func)(const void *data),
                        void (*destroy_func)(void *data)) {
    GenericList *list = (GenericList *) malloc(sizeof(GenericList));
    if (list == NULL) return NULL;

    list->head = NULL;
    list->count = 0;
    list->data_size = data_size;
    list->compare = compare_func;
    list->print = print_func;
    list->destroy = destroy_func;

    return list;
}

// Додавання елемента на початок
int generic_add_front(GenericList *list, const void *data) {
    if (list == NULL || data == NULL) return 0;

    ListNode *new_node = (ListNode *) malloc(sizeof(ListNode));
    if (new_node == NULL) return 0;

    // Копіюємо дані
    new_node->data = malloc(list->data_size);
    if (new_node->data == NULL) {
        free(new_node);
        return 0;
    }

    memcpy(new_node->data, data, list->data_size);
    new_node->next = list->head;
    list->head = new_node;
    list->count++;

    return 1;
}

// Пошук елемента
void* generic_find(GenericList *list, const void *key) {
    if (list == NULL || key == NULL || list->compare == NULL)
        return NULL;

    for (ListNode *current = list->head; current != NULL; current = current->next) {
        if (list->compare(current->data, key) == 0) {
            return current->data;
        }
    }
    return NULL;
}

// Видалення елемента
int generic_remove(GenericList *list, const void *key) {
    if (list == NULL || key == NULL || list->compare == NULL)
        return 0;

    ListNode *current = list->head;
    ListNode *prev = NULL;

    while (current != NULL) {
        if (list->compare(current->data, key) == 0) {
            // Знайшли елемент для видалення
            if (prev == NULL) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }

            // Звільняємо пам'ять
            if (list->destroy != NULL) {
                list->destroy(current->data);
            }
            free(current->data);
            free(current);
            list->count--;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

// Виведення списку
void generic_print(GenericList *list) {
    if (list == NULL || list->print == NULL) {
        printf("Список порожній або функція print не визначена\n");
        return;
    }

    printf("Список (%d елементів):\n", list->count);
    int index = 0;
    for (ListNode *current = list->head; current != NULL; current = current->next) {
        printf("  [%d] ", index++);
        list->print(current->data);
        printf("\n");
    }
}

// Звільнення списку
void generic_destroy(GenericList *list) {
    if (list == NULL) return;

    ListNode *current = list->head;
    while (current != NULL) {
        ListNode *next = current->next;
        if (list->destroy != NULL) {
            list->destroy(current->data);
        }
        free(current->data);
        free(current);
        current = next;
    }
    free(list);
}


int compare_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void print_int(const void *data) {
    printf("int: %d", *(const int*)data);
}

void destroy_int(void *data) {
    // Для простих типів нічого особливого робити не треба
    (void)data; // Щоб компілятор не попереджав про невикористаний параметр
}


typedef struct {
    char *str;
    int length;
} StringData;

int compare_string(const void *a, const void *b) {
    const StringData *sa = (const StringData*)a;
    const StringData *sb = (const StringData*)b;
    return strcmp(sa->str, sb->str);
}

void print_string(const void *data) {
    const StringData *s = (const StringData*)data;
    printf("string: \"%s\" (len=%d)", s->str, s->length);
}

void destroy_string(void *data) {
    StringData *s = (StringData*)data;
    if (s->str != NULL) {
        free(s->str);
        s->str = NULL;
    }
}

StringData create_string_data(const char *str) {
    StringData data;
    data.length = strlen(str);
    data.str = (char *) malloc(data.length + 1);
    strcpy(data.str, str);
    return data;
}


void demonstrate_c_version() {
    printf("=== ДЕМОНСТРАЦІЯ НЕТИПІЗОВАНОГО СПИСКУ НА C ===\n");

    // Тест з int
    printf("\n--- Тест з цілими числами ---\n");
    GenericList *int_list = create_list(sizeof(int), compare_int, print_int, destroy_int);

    int values[] = {42, 17, 89, 3, 56};
    for (int i = 0; i < 5; i++) {
        generic_add_front(int_list, &values[i]);
        printf("Додано: %d\n", values[i]);
    }

    generic_print(int_list);

    // Пошук
    int search_value = 17;
    int *found = (int *) generic_find(int_list, &search_value);
    if (found != NULL) {
        printf("Знайдено: %d\n", *found);
    }

    // Видалення
    int remove_value = 89;
    if (generic_remove(int_list, &remove_value)) {
        printf("Видалено: %d\n", remove_value);
        generic_print(int_list);
    }

    generic_destroy(int_list);

    // Тест з рядками
    printf("\n--- Тест з рядками ---\n");
    GenericList *string_list = create_list(sizeof(StringData),
                                         compare_string, print_string, destroy_string);

    const char *strings[] = {"Alice", "Bob", "Charlie", "David"};
    for (int i = 0; i < 4; i++) {
        StringData data = create_string_data(strings[i]);
        generic_add_front(string_list, &data);
        printf("Додано: %s\n", strings[i]);
    }

    generic_print(string_list);

    // Пошук рядка
    StringData search_data = create_string_data("Bob");
    StringData *found_str = (StringData *) generic_find(string_list, &search_data);
    if (found_str != NULL) {
        printf("Знайдено: %s\n", found_str->str);
    }
    free(search_data.str); // Звільняємо тимчасові дані

    generic_destroy(string_list);
}


#ifdef __cplusplus

#include <iostream>
#include <string>
#include <memory>

template<typename T>
class TypedList {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;

        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    int count;

public:
    TypedList() : head(nullptr), count(0) {}

    ~TypedList() {
        clear();
    }

    // Додавання на початок
    void addFront(const T& value) {
        auto new_node = std::make_unique<Node>(value);
        new_node->next = std::move(head);
        head = std::move(new_node);
        count++;
    }

    // Пошук елемента
    T* find(const T& value) {
        for (Node* current = head.get(); current != nullptr; current = current->next.get()) {
            if (current->data == value) {
                return &(current->data);
            }
        }
        return nullptr;
    }

    // Видалення елемента
    bool remove(const T& value) {
        if (head == nullptr) return false;

        if (head->data == value) {
            head = std::move(head->next);
            count--;
            return true;
        }

        for (Node* current = head.get(); current->next != nullptr; current = current->next.get()) {
            if (current->next->data == value) {
                current->next = std::move(current->next->next);
                count--;
                return true;
            }
        }
        return false;
    }

    // Виведення списку
    void print() const {
        std::cout << "TypedList (" << count << " елементів):\n";
        int index = 0;
        for (Node* current = head.get(); current != nullptr; current = current->next.get()) {
            std::cout << "  [" << index++ << "] " << current->data << "\n";
        }
    }

    // Очищення списку
    void clear() {
        while (head != nullptr) {
            head = std::move(head->next);
        }
        count = 0;
    }

    int size() const { return count; }
    bool empty() const { return count == 0; }
};

void demonstrate_cpp_version() {
    std::cout << "\n=== ДЕМОНСТРАЦІЯ ТИПІЗОВАНОГО ШАБЛОНУ НА C++ ===\n";

    // Тест з int
    std::cout << "\n--- Тест з цілими числами ---\n";
    TypedList<int> int_list;

    int values[] = {42, 17, 89, 3, 56};
    for (int i = 0; i < 5; i++) {
        int_list.addFront(values[i]);
        std::cout << "Додано: " << values[i] << "\n";
    }

    int_list.print();

    // Пошук
    int *found = int_list.find(17);
    if (found != nullptr) {
        std::cout << "Знайдено: " << *found << "\n";
    }

    // Видалення
    if (int_list.remove(89)) {
        std::cout << "Видалено: 89\n";
        int_list.print();
    }

    // Тест з рядками
    std::cout << "\n--- Тест з рядками ---\n";
    TypedList<std::string> string_list;

    std::string strings[] = {"Alice", "Bob", "Charlie", "David"};
    for (int i = 0; i < 4; i++) {
        string_list.addFront(strings[i]);
        std::cout << "Додано: " << strings[i] << "\n";
    }

    string_list.print();

    // Пошук рядка
    std::string *found_str = string_list.find("Bob");
    if (found_str != nullptr) {
        std::cout << "Знайдено: " << *found_str << "\n";
    }
}

#endif // __cplusplus


void comparative_analysis() {
    printf("\n=== ПОРІВНЯЛЬНИЙ АНАЛІЗ C vs C++ ===\n");

    printf("\nC (НЕТИПІЗОВАНИЙ ПІДХІД):\n");
    printf("ПЕРЕВАГИ:\n");
    printf("+ Універсальність - один код для всіх типів\n");
    printf("+ Контроль над пам'яттю\n");
    printf("+ Компактність бінарного коду\n");
    printf("+ Сумісність з C\n");
    printf("+ Менше вимог до компілятора\n\n");

    printf("НЕДОЛІКИ:\n");
    printf("- Відсутність типобезпеки на етапі компіляції\n");
    printf("- Складність у використанні (багато параметрів)\n");
    printf("- Потреба в функціях порівняння/виведення\n");
    printf("- Ручне управління пам'яттю\n");
    printf("- Складність налагодження\n");
    printf("- Можливі помилки касту типів\n\n");

    printf("C++ (ТИПІЗОВАНИЙ ПІДХІД):\n");
    printf("ПЕРЕВАГИ:\n");
    printf("+ Типобезпека на етапі компіляції\n");
    printf("+ Простота використання\n");
    printf("+ Автоматичне управління пам'яттю\n");
    printf("+ Перевантаження операторів\n");
    printf("+ RAII (Resource Acquisition Is Initialization)\n");
    printf("+ Кращі повідомлення про помилки\n");
    printf("+ STL інтеграція\n\n");

    printf("НЕДОЛІКИ:\n");
    printf("- Більший розмір бінарного коду (code bloat)\n");
    printf("- Довший час компіляції\n");
    printf("- Складніші повідомлення про помилки шаблонів\n");
    printf("- Потреба в C++ компіляторі\n");
    printf("- Менший контроль над пам'яттю\n\n");

    printf("ПРОДУКТИВНІСТЬ:\n");
    printf("C версія:   Менший розмір коду, більше runtime перевірок\n");
    printf("C++ версія: Більший розмір коду, більше compile-time оптимізацій\n\n");

    printf("БЕЗПЕКА:\n");
    printf("C версія:   Помилки виявляються під час виконання\n");
    printf("C++ версія: Більшість помилок виявляється під час компіляції\n\n");

    printf("ЗРУЧНІСТЬ ВИКОРИСТАННЯ:\n");
    printf("C версія:   Складна ініціалізація, багато boilerplate коду\n");
    printf("C++ версія: Проста ініціалізація, мінімум зайвого коду\n");
}

void usage_recommendations() {
    printf("\n=== РЕКОМЕНДАЦІЇ ПО ВИКОРИСТАННЮ ===\n");

    printf("ВИКОРИСТОВУЙТЕ C ПІДХІД КОЛИ:\n");
    printf("• Потрібна максимальна сумісність\n");
    printf("• Критично важливий розмір бінарного файлу\n");
    printf("• Робота з embedded системами\n");
    printf("• Необхідний точний контроль над пам'яттю\n");
    printf("• Інтеграція з C бібліотеками\n\n");

    printf("ВИКОРИСТОВУЙТЕ C++ ПІДХІД КОЛИ:\n");
    printf("• Безпека типів критично важлива\n");
    printf("• Швидкість розробки має пріоритет\n");
    printf("• Потрібна інтеграція з STL\n");
    printf("• Команда знайома з C++\n");
    printf("• Складні типи даних з деструкторами\n\n");

    printf("ЗАГАЛЬНА РЕКОМЕНДАЦІЯ:\n");
    printf("Для нових проектів на C++ використовуйте шаблони.\n");
    printf("Для C проектів або legacy коду - void* підхід.\n");
    printf("В обох випадках забезпечте належне тестування!\n");
}

int main(void) {
    printf("=== Завдання 12: Нетипізовані vs Типізовані списки ===\n");

    demonstrate_c_version();

#ifdef __cplusplus
    demonstrate_cpp_version();
#else
    printf("\nC++ версія доступна тільки при компіляції з C++ компілятором\n");
    printf("Для тестування C++ частини скомпілюйте з g++ замість gcc\n");
#endif

    comparative_analysis();
    usage_recommendations();

    return 0;
}