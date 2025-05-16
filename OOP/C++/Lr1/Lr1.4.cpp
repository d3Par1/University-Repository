#include <iostream>
#include <cstring>
#include <cstdlib>

typedef struct Nameval {
    char *name;
    int value;
} Nameval;

typedef struct NVtab {
    Nameval *nameval;
    int size;
    int max;
} NVtab;

#define NVGROW 10

// Безпечна ініціалізація
void init_nvtab(NVtab *nvtptr) {
    nvtptr->nameval = NULL;
    nvtptr->size = 0;
    nvtptr->max = 0;
}

// Безпечна функція додавання (як у прикладі з лекції)
int addname_safe(NVtab *nvtptr, Nameval newname) {
    Nameval *newval;
    
    if (nvtptr->size >= nvtptr->max) {
        // Використовуємо тимчасову змінну для realloc
        newval = (Nameval *) realloc(nvtptr->nameval, 
                                    (nvtptr->max + NVGROW) * sizeof(Nameval));
        if (newval == NULL)
            return -1; // Якщо realloc не вдався, повертаємо помилку
        
        nvtptr->max += NVGROW;
        nvtptr->nameval = newval; // Оновлюємо вказівник лише після успішного realloc
    }
    
    nvtptr->nameval[nvtptr->size] = newname;
    nvtptr->size++;
    
    return nvtptr->size - 1;
}

// Небезпечна функція додавання (пряма підстановка realloc)
int addname_unsafe(NVtab *nvtptr, Nameval newname) {
    if (nvtptr->size >= nvtptr->max) {
        // Небезпечно! Пряма підстановка результату realloc
        nvtptr->nameval = (Nameval *) realloc(nvtptr->nameval, 
                                           (NVGROW * nvtptr->max) * sizeof(Nameval));
        
        if (nvtptr->nameval == NULL) {
            // У випадку помилки realloc, вказівник вже перезаписаний як NULL
            // і всі дані втрачено
            std::cout << "КРИТИЧНА ПОМИЛКА: Втрачено всі дані через неудачу realloc!" << std::endl;
            nvtptr->size = 0;
            nvtptr->max = 0;
            return -1;
        }
        
        nvtptr->max = NVGROW * nvtptr->max; // NVGROW-разове збільшення
    }
    
    nvtptr->nameval[nvtptr->size] = newname;
    nvtptr->size++;
    
    return nvtptr->size - 1;
}

// Функція для демонстрації проблеми з небезпечною реалізацією
void demonstrate_realloc_problem() {
    NVtab safe_tab, unsafe_tab;
    init_nvtab(&safe_tab);
    init_nvtab(&unsafe_tab);
    
    // Заповнюємо обидві таблиці однаковими даними
    for (int i = 0; i < 5; i++) {
        Nameval item;
        char buffer[20];
        sprintf(buffer, "item%d", i);
        
        item.name = strdup(buffer);
        item.value = i + 100;
        
        addname_safe(&safe_tab, item);
        
        // Створюємо копію для другої таблиці
        item.name = strdup(buffer);
        addname_unsafe(&unsafe_tab, item);
    }
    
    std::cout << "Початковий стан:" << std::endl;
    std::cout << "Безпечна таблиця: " << safe_tab.size << " елементів, макс = " << safe_tab.max << std::endl;
    std::cout << "Небезпечна таблиця: " << unsafe_tab.size << " елементів, макс = " << unsafe_tab.max << std::endl;
    
    // Симулюємо ситуацію, коли realloc не вдається (запит на величезний обсяг пам'яті)
    try {
        std::cout << "\nСпроба додати елемент до безпечної таблиці з симульованою помилкою realloc:" << std::endl;
        // Зберігаємо оригінальний вказівник перед спробою
        Nameval *original_ptr = safe_tab.nameval;
        
        // Симулюємо помилку realloc, встановивши величезний max
        safe_tab.max = 1000000000; // Дуже велике значення
        
        Nameval item;
        item.name = strdup("will_fail");
        item.value = 999;
        
        int result = addname_safe(&safe_tab, item);
        
        if (result == -1) {
            std::cout << "Помилка realloc в безпечній реалізації." << std::endl;
            std::cout << "Оригінальні дані збережено." << std::endl;
            
            // Відновлюємо нормальний max
            safe_tab.max = 10;
            
            // Виводимо перший елемент для демонстрації, що дані збережені
            std::cout << "Перший елемент: " << safe_tab.nameval[0].name << std::endl;
        }
    } catch (std::exception &e) {
        std::cout << "Виникло виключення: " << e.what() << std::endl;
    }
    
    try {
        std::cout << "\nСпроба додати елемент до небезпечної таблиці з симульованою помилкою realloc:" << std::endl;
        // Зберігаємо оригінальний вказівник перед спробою
        Nameval *original_ptr = unsafe_tab.nameval;
        
        // Симулюємо помилку realloc, встановивши величезний max
        unsafe_tab.max = 1000000000; // Дуже велике значення
        
        Nameval item;
        item.name = strdup("will_fail");
        item.value = 999;
        
        int result = addname_unsafe(&unsafe_tab, item);
        
        if (result == -1) {
            std::cout << "Помилка realloc в небезпечній реалізації." << std::endl;
            
            if (unsafe_tab.nameval == NULL) {
                std::cout << "Вказівник став NULL! Всі дані втрачено!" << std::endl;
            }
        }
    } catch (std::exception &e) {
        std::cout << "Виникло виключення: " << e.what() << std::endl;
    }
    
    // Звільнення пам'яті
    if (safe_tab.nameval != NULL) {
        for (int i = 0; i < safe_tab.size; i++) {
            free(safe_tab.nameval[i].name);
        }
        free(safe_tab.nameval);
    }
    
    if (unsafe_tab.nameval != NULL) {
        for (int i = 0; i < unsafe_tab.size; i++) {
            free(unsafe_tab.nameval[i].name);
        }
        free(unsafe_tab.nameval);
    } else {
        std::cout << "Неможливо звільнити пам'ять небезпечної таблиці - вказівник NULL!" << std::endl;
    }
}

int main() {
    demonstrate_realloc_problem();
    return 0;
}