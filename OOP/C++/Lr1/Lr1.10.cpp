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

// Ітераційна версія функції reverse_list
Nameval* reverse_list_iterative(Nameval* listp) {
    Nameval *prev = NULL, *current = listp, *next = NULL;
    
    while (current != NULL) {
        // Зберігаємо наступний елемент
        next = current->next;
        
        // Змінюємо напрямок зв'язку поточного елемента
        current->next = prev;
        
        // Переміщуємо вказівники на один крок вперед
        prev = current;
        current = next;
    }
    
    // Тепер prev вказує на новий перший елемент (колишній останній)
    return prev;
}

// Допоміжна рекурсивна функція
Nameval* reverse_list_recursive_helper(Nameval* current, Nameval* prev) {
    if (current == NULL) {
        return prev; // Базовий випадок: досягли кінця списку
    }
    
    // Зберігаємо наступний елемент
    Nameval* next = current->next;
    
    // Змінюємо напрямок зв'язку поточного елемента
    current->next = prev;
    
    // Рекурсивно обробляємо решту списку
    return reverse_list_recursive_helper(next, current);
}

// Рекурсивна версія функції reverse_list
Nameval* reverse_list_recursive(Nameval* listp) {
    return reverse_list_recursive_helper(listp, NULL);
}

int main() {
    // Створюємо тестовий список
    Nameval* list = NULL;
    
    // Додаємо елементи (вони будуть у зворотному порядку через additem)
    list = additem(list, newitem((char*)"item5", 50));
    list = additem(list, newitem((char*)"item4", 40));
    list = additem(list, newitem((char*)"item3", 30));
    list = additem(list, newitem((char*)"item2", 20));
    list = additem(list, newitem((char*)"item1", 10));
    
    std::cout << "Початковий список:" << std::endl;
    print_list(list);
    
    // Створюємо копію списку для демонстрації обох методів
    Nameval* list_copy = NULL;
    for (Nameval* p = list; p != NULL; p = p->next) {
        Nameval* newp = newitem(p->name, p->value);
        list_copy = additem(list_copy, newp);
    }
    
    // Обертаємо порядок елементів ітераційним методом
    std::cout << "\nРезультат ітераційного обертання:" << std::endl;
    Nameval* reversed_iterative = reverse_list_iterative(list);
    print_list(reversed_iterative);
    
    // Обертаємо порядок елементів рекурсивним методом
    std::cout << "\nРезультат рекурсивного обертання:" << std::endl;
    Nameval* reversed_recursive = reverse_list_recursive(list_copy);
    print_list(reversed_recursive);
    
    // Порівняння методів
    std::cout << "\nПорівняння ітераційного та рекурсивного методів:" << std::endl;
    std::cout << "1. Ітераційний метод:" << std::endl;
    std::cout << "   - Використовує константну кількість додаткової пам'яті" << std::endl;
    std::cout << "   - Не ризикує переповненням стеку" << std::endl;
    std::cout << "   - Зазвичай ефективніший для дуже довгих списків" << std::endl;
    std::cout << "   - Може бути менш зрозумілим в складних випадках" << std::endl;
    
    std::cout << "2. Рекурсивний метод:" << std::endl;
    std::cout << "   - Більш елегантний і лаконічний" << std::endl;
    std::cout << "   - Вимагає O(n) додаткової пам'яті в стеку для рекурсивних викликів" << std::endl;
    std::cout << "   - Ризикує переповненням стеку для дуже довгих списків" << std::endl;
    std::cout << "   - Часто простіший для розуміння" << std::endl;
    
    // Звільняємо пам'ять
    freeall(reversed_iterative);
    freeall(reversed_recursive);
    
    return 0;
}