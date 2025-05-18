#include <iostream>
#include <cstring>
#include <cstdlib>

// Структура для зберігання пари ім'я-значення
typedef struct Nameval {
    char *name;
    int value;
    struct Nameval *next; // Вказівник на наступний елемент списку
} Nameval;

// Додавання нового елементу на початок списку
Nameval* additem(Nameval* listp, Nameval* newp) {
    newp->next = listp;
    return newp;
}

// Створення нового елементу
Nameval* newitem(char* name, int value) {
    Nameval* newp = (Nameval*)malloc(sizeof(Nameval));
    if (newp == NULL) {
        return NULL;
    }
    
    newp->name = strdup(name);
    if (newp->name == NULL) {
        free(newp);
        return NULL;
    }
    
    newp->value = value;
    newp->next = NULL;
    return newp;
}

// Пошук елементу за ім'ям
Nameval* lookup(Nameval* listp, char* name) {
    for ( ; listp != NULL; listp = listp->next) {
        if (strcmp(name, listp->name) == 0) {
            return listp;
        }
    }
    return NULL; // Не знайдено
}

// Оригінальна функція видалення елементу (без звільнення поля name)
Nameval* delitem_original(Nameval* listp, char* name) {
    Nameval *p, *prev;
    
    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0) {
            if (prev == NULL) {
                listp = p->next;
            } else {
                prev->next = p->next;
            }
            // Тут не звільняється пам'ять для p->name
            free(p);
            return listp;
        }
        prev = p;
    }
    
    return listp; // Не знайдено
}

// Модифікована функція видалення елементу (зі звільненням поля name)
Nameval* delitem(Nameval* listp, char* name) {
    Nameval *p, *prev;
    
    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0) {
            if (prev == NULL) {
                listp = p->next;
            } else {
                prev->next = p->next;
            }
            // Звільнення пам'яті для поля name
            free(p->name);
            free(p);
            return listp;
        }
        prev = p;
    }
    
    return listp; // Не знайдено
}

// Оригінальна функція звільнення всього списку (без звільнення полів name)
void freeall_original(Nameval* listp) {
    Nameval *next;
    
    for ( ; listp != NULL; listp = next) {
        next = listp->next;
        // Не звільняється пам'ять для listp->name
        free(listp);
    }
}

// Модифікована функція звільнення всього списку (зі звільненням полів name)
void freeall(Nameval* listp) {
    Nameval *next;
    
    for ( ; listp != NULL; listp = next) {
        next = listp->next;
        // Звільнення пам'яті для поля name
        free(listp->name);
        free(listp);
    }
}

// Функція для виведення елементів списку
void print_list(Nameval* listp) {
    int count = 0;
    
    for ( ; listp != NULL; listp = listp->next) {
        std::cout << count << ". " << listp->name << " = " << listp->value << std::endl;
        count++;
    }
    
    std::cout << "Всього елементів: " << count << std::endl;
}

// Функція для перевірки пам'яті (для демонстраційних цілей)
// В реальному житті використовувався б інструмент типу Valgrind
void check_memory_leaks() {
    std::cout << "У реальному середовищі тут би використовувався Valgrind або інший" << std::endl;
    std::cout << "інструмент для виявлення витоків пам'яті." << std::endl;
}

// Демонстрація проблеми без звільнення name
void demonstrate_memory_leak() {
    Nameval* list = NULL;
    
    // Створюємо список
    for (int i = 0; i < 5; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        // Виділяємо пам'ять для name через strdup у newitem
        list = additem(list, newitem(buffer, i * 10));
    }
    
    std::cout << "Створено список:" << std::endl;
    print_list(list);
    
    // Видаляємо елемент без звільнення name
    std::cout << "\nВидаляємо 'item2' без звільнення name:" << std::endl;
    list = delitem_original(list, (char*)"item2");
    print_list(list);
    
    std::cout << "\nРезультат перевірки на витоки пам'яті:" << std::endl;
    std::cout << "Є витік пам'яті, оскільки поле name елемента 'item2' не було звільнено." << std::endl;
    check_memory_leaks();
    
    // Звільняємо весь список без звільнення name
    std::cout << "\nЗвільняємо весь список без звільнення name:" << std::endl;
    freeall_original(list);
    
    std::cout << "\nРезультат перевірки на витоки пам'яті:" << std::endl;
    std::cout << "Є витік пам'яті, оскільки поля name всіх елементів не були звільнені." << std::endl;
    check_memory_leaks();
}

// Демонстрація правильного звільнення пам'яті
void demonstrate_correct_memory_release() {
    Nameval* list = NULL;
    
    // Створюємо список
    for (int i = 0; i < 5; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        list = additem(list, newitem(buffer, i * 10));
    }
    
    std::cout << "Створено список:" << std::endl;
    print_list(list);
    
    // Видаляємо елемент зі звільненням name
    std::cout << "\nВидаляємо 'item2' зі звільненням name:" << std::endl;
    list = delitem(list, (char*)"item2");
    print_list(list);
    
    std::cout << "\nРезультат перевірки на витоки пам'яті:" << std::endl;
    std::cout << "Витоків пам'яті немає, оскільки поле name елемента 'item2' було звільнено." << std::endl;
    check_memory_leaks();
    
    // Звільняємо весь список зі звільненням name
    std::cout << "\nЗвільняємо весь список зі звільненням name:" << std::endl;
    freeall(list);
    
    std::cout << "\nРезультат перевірки на витоки пам'яті:" << std::endl;
    std::cout << "Витоків пам'яті немає, оскільки поля name всіх елементів були звільнені." << std::endl;
    check_memory_leaks();
}

int main() {
    std::cout << "=== Демонстрація проблеми без звільнення полів name ===" << std::endl;
    demonstrate_memory_leak();
    
    std::cout << "\n=== Демонстрація правильного звільнення пам'яті ===" << std::endl;
    demonstrate_correct_memory_release();
    
    std::cout << "\n=== Пояснення змін у функціях ===" << std::endl;
    std::cout << "1. Функція delitem була модифікована для звільнення поля name:" << std::endl;
    std::cout << "   - Додано виклик free(p->name) перед free(p)" << std::endl;
    std::cout << "2. Функція freeall була модифікована для звільнення полів name:" << std::endl;
    std::cout << "   - Додано виклик free(listp->name) перед free(listp)" << std::endl;
    std::cout << "3. Ці зміни запобігають витокам пам'яті, оскільки:" << std::endl;
    std::cout << "   - Пам'ять для поля name виділяється окремо через strdup()" << std::endl;
    std::cout << "   - При видаленні елемента потрібно звільнити всю пам'ять," << std::endl;
    std::cout << "     включно з пам'яттю для його полів" << std::endl;
    
    return 0;
}