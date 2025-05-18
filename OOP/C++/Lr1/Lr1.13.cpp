#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cassert>

// Структура для зберігання пари ім'я-значення
typedef struct Nameval {
    char *name;
    int value;
    struct Nameval *next; // Вказівник на наступний елемент списку
} Nameval;

// Функції для роботи зі списком

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

// Додавання елементу в кінець списку
Nameval* append(Nameval* listp, Nameval* newp) {
    Nameval* p;
    
    // Якщо список порожній
    if (listp == NULL) {
        return newp;
    }
    
    // Знаходимо останній елемент
    for (p = listp; p->next != NULL; p = p->next)
        ;
    
    // Додаємо новий елемент в кінець
    p->next = newp;
    return listp;
}

// Підрахунок кількості елементів у списку
int count_items(Nameval* listp) {
    int count = 0;
    
    for ( ; listp != NULL; listp = listp->next) {
        count++;
    }
    
    return count;
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

// ============= Тестувальний код =============

// Допоміжні функції для тестування

// Створює список з заданою кількістю елементів
Nameval* create_test_list(int n) {
    Nameval* list = NULL;
    
    for (int i = 0; i < n; i++) {
        char buf[20];
        sprintf(buf, "item%d", i);
        Nameval* newp = newitem(buf, i * 10);
        list = append(list, newp);
    }
    
    return list;
}

// Порівнює два списки та повертає true, якщо вони ідентичні
bool compare_lists(Nameval* list1, Nameval* list2) {
    while (list1 != NULL && list2 != NULL) {
        if (strcmp(list1->name, list2->name) != 0 || list1->value != list2->value) {
            return false;
        }
        list1 = list1->next;
        list2 = list2->next;
    }
    
    // Обидва списки повинні закінчуватися одночасно
    return list1 == NULL && list2 == NULL;
}

// Тести для функцій роботи зі списком

// Тест для функції newitem
void test_newitem() {
    std::cout << "Тест функції newitem:" << std::endl;
    
    // Тест створення елемента
    Nameval* item = newitem((char*)"test", 42);
    assert(item != NULL);
    assert(strcmp(item->name, "test") == 0);
    assert(item->value == 42);
    assert(item->next == NULL);
    
    // Звільняємо пам'ять
    free(item->name);
    free(item);
    
    std::cout << "  Успішно: Елемент створено правильно" << std::endl;
}

// Тест для функції additem
void test_additem() {
    std::cout << "Тест функції additem:" << std::endl;
    
    // Створюємо елементи
    Nameval* item1 = newitem((char*)"item1", 10);
    Nameval* item2 = newitem((char*)"item2", 20);
    Nameval* item3 = newitem((char*)"item3", 30);
    
    // Додаємо елементи у зворотному порядку
    Nameval* list = NULL;
    list = additem(list, item1); // list: item1
    assert(list == item1);
    assert(list->next == NULL);
    
    list = additem(list, item2); // list: item2 -> item1
    assert(list == item2);
    assert(list->next == item1);
    assert(list->next->next == NULL);
    
    list = additem(list, item3); // list: item3 -> item2 -> item1
    assert(list == item3);
    assert(list->next == item2);
    assert(list->next->next == item1);
    assert(list->next->next->next == NULL);
    
    // Перевіряємо значення
    assert(strcmp(list->name, "item3") == 0);
    assert(list->value == 30);
    assert(strcmp(list->next->name, "item2") == 0);
    assert(list->next->value == 20);
    assert(strcmp(list->next->next->name, "item1") == 0);
    assert(list->next->next->value == 10);
    
    // Звільняємо пам'ять
    freeall(list);
    
    std::cout << "  Успішно: Елементи додані правильно" << std::endl;
}

// Тест для функції lookup
void test_lookup() {
    std::cout << "Тест функції lookup:" << std::endl;
    
    // Створюємо тестовий список
    Nameval* list = create_test_list(5); // item0, item1, item2, item3, item4
    
    // Перевіряємо пошук існуючих елементів
    Nameval* found = lookup(list, (char*)"item0");
    assert(found != NULL);
    assert(strcmp(found->name, "item0") == 0);
    assert(found->value == 0);
    
    found = lookup(list, (char*)"item2");
    assert(found != NULL);
    assert(strcmp(found->name, "item2") == 0);
    assert(found->value == 20);
    
    found = lookup(list, (char*)"item4");
    assert(found != NULL);
    assert(strcmp(found->name, "item4") == 0);
    assert(found->value == 40);
    
    // Перевіряємо пошук неіснуючого елемента
    found = lookup(list, (char*)"non_existent");
    assert(found == NULL);
    
    // Звільняємо пам'ять
    freeall(list);
    
    std::cout << "  Успішно: Пошук елементів працює правильно" << std::endl;
}

// Тест для функції delitem
void test_delitem() {
    std::cout << "Тест функції delitem:" << std::endl;
    
    // Тест на видалення першого елемента
    {
        Nameval* list = create_test_list(3); // item0, item1, item2
        assert(count_items(list) == 3);
        
        list = delitem(list, (char*)"item0");
        assert(count_items(list) == 2);
        assert(lookup(list, (char*)"item0") == NULL);
        assert(lookup(list, (char*)"item1") != NULL);
        assert(lookup(list, (char*)"item2") != NULL);
        
        freeall(list);
    }
    
    // Тест на видалення середнього елемента
    {
        Nameval* list = create_test_list(3); // item0, item1, item2
        assert(count_items(list) == 3);
        
        list = delitem(list, (char*)"item1");
        assert(count_items(list) == 2);
        assert(lookup(list, (char*)"item0") != NULL);
        assert(lookup(list, (char*)"item1") == NULL);
        assert(lookup(list, (char*)"item2") != NULL);
        
        freeall(list);
    }
    
    // Тест на видалення останнього елемента
    {
        Nameval* list = create_test_list(3); // item0, item1, item2
        assert(count_items(list) == 3);
        
        list = delitem(list, (char*)"item2");
        assert(count_items(list) == 2);
        assert(lookup(list, (char*)"item0") != NULL);
        assert(lookup(list, (char*)"item1") != NULL);
        assert(lookup(list, (char*)"item2") == NULL);
        
        freeall(list);
    }
    
    // Тест на видалення неіснуючого елемента
    {
        Nameval* list = create_test_list(3); // item0, item1, item2
        int original_count = count_items(list);
        
        list = delitem(list, (char*)"non_existent");
        assert(count_items(list) == original_count);
        
        freeall(list);
    }
    
    // Тест на видалення з порожнього списку
    {
        Nameval* list = NULL;
        list = delitem(list, (char*)"any");
        assert(list == NULL);
    }
    
    std::cout << "  Успішно: Видалення елементів працює правильно" << std::endl;
}

// Тест для функції append
void test_append() {
    std::cout << "Тест функції append:" << std::endl;
    
    // Тест додавання до порожнього списку
    {
        Nameval* list = NULL;
        Nameval* item = newitem((char*)"test", 42);
        
        list = append(list, item);
        assert(list == item);
        assert(list->next == NULL);
        
        freeall(list);
    }
    
    // Тест додавання до непорожнього списку
    {
        Nameval* list = create_test_list(3); // item0, item1, item2
        Nameval* item = newitem((char*)"item3", 30);
        
        int original_count = count_items(list);
        list = append(list, item);
        assert(count_items(list) == original_count + 1);
        
        // Перевіряємо, що item3 останній у списку
        Nameval* last = list;
        while (last->next != NULL) {
            last = last->next;
        }
        assert(strcmp(last->name, "item3") == 0);
        assert(last->value == 30);
        
        freeall(list);
    }
    
    std::cout << "  Успішно: Додавання в кінець списку працює правильно" << std::endl;
}

// Тест для функції count_items
void test_count_items() {
    std::cout << "Тест функції count_items:" << std::endl;
    
    // Тест на порожньому списку
    {
        Nameval* list = NULL;
        assert(count_items(list) == 0);
    }
    
    // Тест на списку з одним елементом
    {
        Nameval* list = newitem((char*)"single", 1);
        assert(count_items(list) == 1);
        freeall(list);
    }
    
    // Тест на списку з кількома елементами
    {
        Nameval* list = create_test_list(5); // item0, item1, item2, item3, item4
        assert(count_items(list) == 5);
        freeall(list);
    }
    
    std::cout << "  Успішно: Підрахунок елементів працює правильно" << std::endl;
}

// Виконання всіх тестів
void run_all_tests() {
    std::cout << "Запуск всіх тестів для функцій роботи зі списком\n" << std::endl;
    
    test_newitem();
    test_additem();
    test_lookup();
    test_delitem();
    test_append();
    test_count_items();
    
    std::cout << "\nВсі тести успішно пройдені!" << std::endl;
}

int main() {
    run_all_tests();
    return 0;
}