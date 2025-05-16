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

// Функція для перебору елементів списку
void coolfun(Nameval* listp, void (*fn)(Nameval*, void*), void* arg) {
    for ( ; listp != NULL; listp = listp->next) {
        // Викликаємо передану функцію для кожного елемента списку
        (*fn)(listp, arg);
    }
}

// Приклад функції, яка інкрементує лічильник
void inccounter(Nameval* p, void* arg) {
    int* ip;
    ip = (int*)arg;
    (*ip)++;
}

// Функція, яка виводить елемент списку
void printitem(Nameval* p, void* arg) {
    int* index = (int*)arg;
    std::cout << *index << ". " << p->name << " = " << p->value << std::endl;
    (*index)++;
}

// Функція, яка змінює значення елемента
void increment_value(Nameval* p, void* arg) {
    int increment = *(int*)arg;
    p->value += increment;
}

// Функція, яка накопичує суму значень
void sum_values(Nameval* p, void* arg) {
    int* sum = (int*)arg;
    *sum += p->value;
}

// Функція, яка шукає елемент з максимальним значенням
void find_max(Nameval* p, void* arg) {
    Nameval** max_item = (Nameval**)arg;
    
    if (*max_item == NULL || p->value > (*max_item)->value) {
        *max_item = p;
    }
}

// Функція для звільнення всього списку
void freeall(Nameval* listp) {
    Nameval *next;
    
    for ( ; listp != NULL; listp = next) {
        next = listp->next;
        free(listp->name);
        free(listp);
    }
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
    
    // Підрахунок елементів
    int n = 0;
    coolfun(nvlist, inccounter, &n);
    std::cout << "У списку " << n << " елементів" << std::endl;
    
    // Виведення елементів
    std::cout << "\nЕлементи списку:" << std::endl;
    int index = 0;
    coolfun(nvlist, printitem, &index);
    
    // Збільшення всіх значень на 5
    std::cout << "\nЗбільшуємо всі значення на 5:" << std::endl;
    int increment = 5;
    coolfun(nvlist, increment_value, &increment);
    
    // Виводимо оновлені значення
    index = 0;
    coolfun(nvlist, printitem, &index);
    
    // Обчислення суми всіх значень
    int sum = 0;
    coolfun(nvlist, sum_values, &sum);
    std::cout << "\nСума всіх значень: " << sum << std::endl;
    
    // Знаходження елемента з максимальним значенням
    Nameval* max_item = NULL;
    coolfun(nvlist, find_max, &max_item);
    if (max_item != NULL) {
        std::cout << "\nЕлемент з максимальним значенням: " 
                  << max_item->name << " = " << max_item->value << std::endl;
    }
    
    // Звільнення всього списку
    freeall(nvlist);
    
    return 0;
}