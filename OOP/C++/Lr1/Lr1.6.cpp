#include <iostream>
#include <cstring>
#include <cstdlib>

typedef struct Nameval {
    char *name;
    int value;
} Nameval;

typedef struct NVtab {
    Nameval *nameval;
    int size;         // Розмір масиву (включаючи активні та неактивні елементи)
    int max;          // Максимальний розмір масиву
    int active;       // Кількість активних елементів
} NVtab;

#define NVGROW 10

// Ініціалізація масиву
void init_nvtab(NVtab *nvtptr) {
    nvtptr->nameval = NULL;
    nvtptr->size = 0;
    nvtptr->max = 0;
    nvtptr->active = 0;
}

// Модифікована функція додавання, яка спочатку шукає вільну позицію
int addname(NVtab *nvtptr, Nameval newname) {
    Nameval *newval;
    int i;
    
    // Спочатку шукаємо вільну позицію (елемент з name == NULL)
    for (i = 0; i < nvtptr->size; i++) {
        if (nvtptr->nameval[i].name == NULL) {
            // Знайдено вільну позицію
            nvtptr->nameval[i] = newname;
            nvtptr->active++;
            return i;
        }
    }
    
    // Вільної позиції не знайдено, перевіряємо, чи потрібно розширити масив
    if (nvtptr->size >= nvtptr->max) {
        newval = (Nameval *) realloc(nvtptr->nameval, 
                                    (nvtptr->max + NVGROW) * sizeof(Nameval));
        if (newval == NULL)
            return -1;
        
        nvtptr->max += NVGROW;
        nvtptr->nameval = newval;
    }
    
    // Додаємо елемент в кінець масиву
    nvtptr->nameval[nvtptr->size] = newname;
    nvtptr->size++;
    nvtptr->active++;
    
    return nvtptr->size - 1;
}

// Модифікована функція видалення, яка позначає елементи як невикористовувані
int delname(NVtab *nvtptr, char *name) {
    int i;
    
    for (i = 0; i < nvtptr->size; i++) {
        // Перевіряємо, чи не є елемент уже видаленим
        if (nvtptr->nameval[i].name != NULL && strcmp(nvtptr->nameval[i].name, name) == 0) {
            // Знайдено елемент - звільняємо пам'ять імені
            free(nvtptr->nameval[i].name);
            
            // Позначаємо як невикористовуваний
            nvtptr->nameval[i].name = NULL;
            nvtptr->active--;
            
            return 1;
        }
    }
    
    return 0;
}

// Функція для виведення всіх елементів масиву, включаючи невикористовувані
void print_all_elements(NVtab *nvtptr) {
    std::cout << "Всі елементи масиву (загальна кількість: " << nvtptr->size 
              << ", активних: " << nvtptr->active << "):" << std::endl;
    
    for (int i = 0; i < nvtptr->size; i++) {
        if (nvtptr->nameval[i].name != NULL) {
            std::cout << "[" << i << "] Активний: " << nvtptr->nameval[i].name 
                      << " = " << nvtptr->nameval[i].value << std::endl;
        } else {
            std::cout << "[" << i << "] <вільна позиція>" << std::endl;
        }
    }
    std::cout << std::endl;
}

// Функція для виведення тільки активних елементів
void print_active_elements(NVtab *nvtptr) {
    std::cout << "Активні елементи масиву (всього: " << nvtptr->active << "):" << std::endl;
    
    int count = 0;
    for (int i = 0; i < nvtptr->size; i++) {
        if (nvtptr->nameval[i].name != NULL) {
            std::cout << "[" << i << "] " << nvtptr->nameval[i].name 
                      << " = " << nvtptr->nameval[i].value << std::endl;
            count++;
        }
    }
    std::cout << std::endl;
}

int main() {
    NVtab nvtab;
    init_nvtab(&nvtab);
    
    // Додаємо елементи
    Nameval item;
    for (int i = 1; i <= 5; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        item.name = strdup(buffer);
        item.value = i * 10;
        addname(&nvtab, item);
    }
    
    std::cout << "Початковий стан масиву:" << std::endl;
    print_all_elements(&nvtab);
    
    // Видаляємо деякі елементи
    std::cout << "Видаляємо 'item2' та 'item4':" << std::endl;
    delname(&nvtab, (char*)"item2");
    delname(&nvtab, (char*)"item4");
    print_all_elements(&nvtab);
    
    // Додаємо нові елементи
    std::cout << "Додаємо нові елементи 'new1' та 'new2':" << std::endl;
    item.name = strdup("new1");
    item.value = 100;
    addname(&nvtab, item);
    
    item.name = strdup("new2");
    item.value = 200;
    addname(&nvtab, item);
    
    print_all_elements(&nvtab);
    
    // Виводимо тільки активні елементи
    std::cout << "Відображення тільки активних елементів:" << std::endl;
    print_active_elements(&nvtab);
    
    // Аналіз ізоляції змін
    std::cout << "Аналіз ізоляції змін:" << std::endl;
    std::cout << "1. Інша частина програми, яка використовує функції addname та delname," << std::endl;
    std::cout << "   не потребує змін, оскільки інтерфейс функцій залишився тим самим." << std::endl;
    std::cout << "2. Єдина зміна в структурі NVtab - додавання поля 'active'." << std::endl;
    std::cout << "3. Зміни локалізовані в реалізації функцій, а не в інтерфейсі." << std::endl;
    
    // Звільнення пам'яті
    for (int i = 0; i < nvtab.size; i++) {
        if (nvtab.nameval[i].name != NULL) {
            free(nvtab.nameval[i].name);
        }
    }
    free(nvtab.nameval);
    
    return 0;
}