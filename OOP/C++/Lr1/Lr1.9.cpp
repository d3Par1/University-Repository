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

// Копіювання списку
Nameval* copy_list(Nameval* listp) {
    Nameval* head = NULL;
    Nameval* tail = NULL;
    Nameval* newp;
    
    // Проходимо список у прямому порядку
    for ( ; listp != NULL; listp = listp->next) {
        // Створюємо новий елемент
        newp = newitem(listp->name, listp->value);
        if (newp == NULL) {
            // Невдача при створенні елемента - звільняємо все, що було вже створено
            Nameval *next;
            for (Nameval* p = head; p != NULL; p = next) {
                next = p->next;
                free(p->name);
                free(p);
            }
            return NULL;
        }
        
        // Додаємо елемент до кінця нового списку
        if (head == NULL) {
            head = newp;
        } else {
            tail->next = newp;
        }
        tail = newp;
    }
    
    return head;
}

// Злиття двох списків
Nameval* merge_lists(Nameval* list1, Nameval* list2) {
    if (list1 == NULL) {
        return copy_list(list2);
    }
    
    if (list2 == NULL) {
        return copy_list(list1);
    }
    
    // Копіюємо перший список
    Nameval* result = copy_list(list1);
    
    // Знаходимо кінець першого списку
    Nameval* tail = result;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    
    // Додаємо копію другого списку
    tail->next = copy_list(list2);
    
    return result;
}

// Розбиття списку на дві частини по заданому елементу
// Повертає покажчик на початок другої частини, модифікує первинний список
Nameval* split_list(Nameval* listp, char* name) {
    Nameval *p, *prev;
    
    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0) {
            // Знайдено елемент для розбиття
            if (prev == NULL) {
                // Це перший елемент, тоді просто повертаємо весь список
                return listp;
            } else {
                // Розриваємо зв'язок
                prev->next = NULL;
                return p;
            }
        }
        prev = p;
    }
    
    return NULL; // Елемент не знайдено
}

// Вставка елемента перед зазначеним елементом
Nameval* insert_before(Nameval* listp, char* target, Nameval* newp) {
    Nameval *p, *prev;
    
    // Якщо список порожній, просто додаємо елемент
    if (listp == NULL) {
        return newp;
    }
    
    // Якщо треба вставити перед першим елементом
    if (strcmp(target, listp->name) == 0) {
        newp->next = listp;
        return newp;
    }
    
    // Шукаємо елемент для вставки перед ним
    prev = listp;
    for (p = listp->next; p != NULL; p = p->next) {
        if (strcmp(target, p->name) == 0) {
            // Знайдено елемент для вставки перед ним
            prev->next = newp;
            newp->next = p;
            return listp;
        }
        prev = p;
    }
    
    // Елемент не знайдено, додаємо новий елемент в кінець
    prev->next = newp;
    return listp;
}

// Вставка елемента після зазначеного елемента
Nameval* insert_after(Nameval* listp, char* target, Nameval* newp) {
    Nameval *p;
    
    // Якщо список порожній, просто додаємо елемент
    if (listp == NULL) {
        return newp;
    }
    
    // Шукаємо елемент для вставки після нього
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(target, p->name) == 0) {
            // Знайдено елемент для вставки після нього
            newp->next = p->next;
            p->next = newp;
            return listp;
        }
    }
    
    // Елемент не знайдено, додаємо новий елемент в кінець
    p = listp;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = newp;
    return listp;
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

// Звільнення всього списку
void freeall(Nameval* listp) {
    Nameval *next;
    
    for ( ; listp != NULL; listp = next) {
        next = listp->next;
        free(listp->name);
        free(listp);
    }
}

int main() {
    // Демонстрація операцій зі списком
    
    // Створення першого списку
    Nameval* list1 = NULL;
    list1 = additem(list1, newitem((char*)"item1", 10));
    list1 = additem(list1, newitem((char*)"item2", 20));
    list1 = additem(list1, newitem((char*)"item3", 30));
    
    std::cout << "Список 1:" << std::endl;
    print_list(list1);
    
    // Копіювання списку
    std::cout << "\nКопіювання списку:" << std::endl;
    Nameval* list_copy = copy_list(list1);
    print_list(list_copy);
    
    // Створення другого списку
    Nameval* list2 = NULL;
    list2 = additem(list2, newitem((char*)"item4", 40));
    list2 = additem(list2, newitem((char*)"item5", 50));
    
    std::cout << "\nСписок 2:" << std::endl;
    print_list(list2);
    
    // Злиття списків
    std::cout << "\nЗлиття списків:" << std::endl;
    Nameval* merged_list = merge_lists(list1, list2);
    print_list(merged_list);
    
    // Вставка перед елементом
    std::cout << "\nВставка перед елементом 'item2':" << std::endl;
    list1 = insert_before(list1, (char*)"item2", newitem((char*)"item_before", 15));
    print_list(list1);
    
    // Вставка після елемента
    std::cout << "\nВставка після елемента 'item3':" << std::endl;
    list1 = insert_after(list1, (char*)"item3", newitem((char*)"item_after", 35));
    print_list(list1);
    
    // Розбиття списку
    std::cout << "\nРозбиття списку по елементу 'item2':" << std::endl;
    Nameval* split_part = split_list(list1, (char*)"item2");
    
    std::cout << "Перша частина:" << std::endl;
    print_list(list1);
    
    std::cout << "Друга частина:" << std::endl;
    print_list(split_part);
    
    // Аналіз складності операцій вставки
    std::cout << "\nАналіз складності операцій вставки:" << std::endl;
    std::cout << "1. Вставка перед елементом потребує пошуку елемента та" << std::endl;
    std::cout << "   попереднього елемента, тобто має складність O(n)." << std::endl;
    std::cout << "2. Вставка після елемента вимагає лише знаходження самого елемента," << std::endl;
    std::cout << "   тобто також має складність O(n), але потребує менше операцій" << std::endl;
    std::cout << "   порівняно з вставкою перед елементом." << std::endl;
    std::cout << "3. Вставка перед першим елементом має складність O(1) для обох методів," << std::endl;
    std::cout << "   але вставка після останнього елемента потребує додаткового обходу" << std::endl;
    std::cout << "   для пошуку останнього елемента, якщо елемент не знайдено." << std::endl;
    
    // Звільнення всіх списків
    freeall(list1);
    freeall(list2);
    freeall(list_copy);
    freeall(merged_list);
    freeall(split_part);
    
    return 0;
}