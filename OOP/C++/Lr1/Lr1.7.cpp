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

// Видалення елементу з списку
Nameval* delitem(Nameval* listp, char* name) {
    Nameval *p, *prev;
    
    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0) {
            if (prev == NULL) {
                // Видаляємо перший елемент списку
                listp = p->next;
            } else {
                // Видаляємо елемент з середини або з кінця
                prev->next = p->next;
            }
            free(p->name);
            free(p);
            return listp;
        }
        prev = p;
    }
    
    return listp; // Елемент не знайдено
}

// Звільнення всього списку
void freeall(Nameval* listp) {
    Nameval *next;
    
    for ( ; listp != NULL; listp = next) {
        next = listp->next;
        free(listp->name);
        free(listp);
    }
}

// Функція для виведення всіх елементів списку
void print_list(Nameval* listp) {
    int count = 0;
    
    std::cout << "Елементи списку:" << std::endl;
    
    for ( ; listp != NULL; listp = listp->next) {
        std::cout << count << ". " << listp->name << " = " << listp->value << std::endl;
        count++;
    }
    
    std::cout << "Всього елементів: " << count << std::endl;
}

// Функція для обчислення довжини списку
int list_length(Nameval* listp) {
    int count = 0;
    
    for ( ; listp != NULL; listp = listp->next) {
        count++;
    }
    
    return count;
}

int main() {
    Nameval* nvlist = NULL;
    Nameval* newp;
    
    // Додаємо елементи до списку
    newp = newitem((char*)"item1", 10);
    nvlist = additem(nvlist, newp);
    
    newp = newitem((char*)"item2", 20);
    nvlist = additem(nvlist, newp);
    
    newp = newitem((char*)"item3", 30);
    nvlist = additem(nvlist, newp);
    
    newp = newitem((char*)"item4", 40);
    nvlist = additem(nvlist, newp);
    
    newp = newitem((char*)"item5", 50);
    nvlist = additem(nvlist, newp);
    
    // Виводимо початковий список
    std::cout << "Початковий список:" << std::endl;
    print_list(nvlist);
    
    // Демонструємо функцію обчислення довжини
    std::cout << "\nДовжина списку: " << list_length(nvlist) << std::endl;
    
    // Пошук елементу
    Nameval* found = lookup(nvlist, (char*)"item3");
    if (found != NULL) {
        std::cout << "\nЗнайдено елемент: " << found->name << " = " << found->value << std::endl;
    } else {
        std::cout << "\nЕлемент не знайдено" << std::endl;
    }
    
    // Видалення елементу
    std::cout << "\nВидалення елементу 'item3':" << std::endl;
    nvlist = delitem(nvlist, (char*)"item3");
    print_list(nvlist);
    
    // Звільнення всього списку
    freeall(nvlist);
    
    return 0;
}