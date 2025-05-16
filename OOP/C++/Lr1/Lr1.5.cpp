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

// Ініціалізація масиву
void init_nvtab(NVtab *nvtptr) {
    nvtptr->nameval = NULL;
    nvtptr->size = 0;
    nvtptr->max = 0;
}

// Функція додавання елемента
int addname(NVtab *nvtptr, Nameval newname) {
    Nameval *newval;
    
    if (nvtptr->size >= nvtptr->max) {
        newval = (Nameval *) realloc(nvtptr->nameval, 
                                    (nvtptr->max + NVGROW) * sizeof(Nameval));
        if (newval == NULL)
            return -1;
        
        nvtptr->max += NVGROW;
        nvtptr->nameval = newval;
    }
    
    nvtptr->nameval[nvtptr->size] = newname;
    nvtptr->size++;
    
    return nvtptr->size - 1;
}

// Стандартна функція видалення (зі зсувом елементів)
int delname_shift(NVtab *nvtptr, char *name) {
    int i;
    
    for (i = 0; i < nvtptr->size; i++) {
        if (strcmp(nvtptr->nameval[i].name, name) == 0) {
            // Зсув елементів
            memmove(&nvtptr->nameval[i], &nvtptr->nameval[i+1], 
                   (nvtptr->size - i - 1) * sizeof(Nameval));
            nvtptr->size--;
            return 1;
        }
    }
    
    return 0;
}

// Функція видалення з перекиданням останнього елемента у вільну позицію
int delname_swap(NVtab *nvtptr, char *name) {
    int i;
    
    for (i = 0; i < nvtptr->size; i++) {
        if (strcmp(nvtptr->nameval[i].name, name) == 0) {
            // Замість зсуву елементів, просто копіюємо останній елемент у вільну позицію
            // (якщо знайдений елемент не є останнім)
            if (i < nvtptr->size - 1) {
                // Звільняємо пам'ять від імені елемента, що видаляється
                free(nvtptr->nameval[i].name);
                
                // Копіюємо останній елемент у вільну позицію
                nvtptr->nameval[i] = nvtptr->nameval[nvtptr->size - 1];
            }
            
            // Зменшуємо розмір масиву
            nvtptr->size--;
            return 1;
        }
    }
    
    return 0;
}

// Функція для виведення елементів масиву
void print_elements(NVtab *nvtptr) {
    std::cout << "Елементи масиву (загалом " << nvtptr->size << "):" << std::endl;
    for (int i = 0; i < nvtptr->size; i++) {
        std::cout << "[" << i << "] " << nvtptr->nameval[i].name 
                  << " = " << nvtptr->nameval[i].value << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    // Демонстрація обох варіантів видалення елементів
    
    // 1. Варіант зі зсувом елементів (зберігає порядок)
    NVtab shift_tab;
    init_nvtab(&shift_tab);
    
    // Додаємо елементи
    Nameval item;
    for (int i = 1; i <= 5; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        item.name = strdup(buffer);
        item.value = i * 10;
        addname(&shift_tab, item);
    }
    
    std::cout << "Демонстрація видалення зі зсувом (збереження порядку):" << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
    std::cout << "Початковий стан:" << std::endl;
    print_elements(&shift_tab);
    
    // Видаляємо середній елемент (item3)
    delname_shift(&shift_tab, (char*)"item3");
    
    std::cout << "Після видалення 'item3' (зі зсувом):" << std::endl;
    print_elements(&shift_tab);
    
    // 2. Варіант з перекиданням останнього елемента (не зберігає порядок)
    NVtab swap_tab;
    init_nvtab(&swap_tab);
    
    // Додаємо такі ж елементи
    for (int i = 1; i <= 5; i++) {
        char buffer[20];
        sprintf(buffer, "item%d", i);
        item.name = strdup(buffer);
        item.value = i * 10;
        addname(&swap_tab, item);
    }
    
    std::cout << "\nДемонстрація видалення з перекиданням (не зберігає порядок):" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "Початковий стан:" << std::endl;
    print_elements(&swap_tab);
    
    // Видаляємо середній елемент (item3)
    delname_swap(&swap_tab, (char*)"item3");
    
    std::cout << "Після видалення 'item3' (з перекиданням):" << std::endl;
    print_elements(&swap_tab);
    
    // Додаємо ще одне видалення для наочності
    delname_swap(&swap_tab, (char*)"item2");
    
    std::cout << "Після видалення 'item2' (з перекиданням):" << std::endl;
    print_elements(&swap_tab);
    
    // Звільнення пам'яті
    for (int i = 0; i < shift_tab.size; i++) {
        free(shift_tab.nameval[i].name);
    }
    free(shift_tab.nameval);
    
    for (int i = 0; i < swap_tab.size; i++) {
        free(swap_tab.nameval[i].name);
    }
    free(swap_tab.nameval);
    
    return 0;
}