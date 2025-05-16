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

// Додавання елемента
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

// Видалення елемента
int delname(NVtab *nvtptr, char *name) {
    int i;
    
    for (i = 0; i < nvtptr->size; i++) {
        if (strcmp(nvtptr->nameval[i].name, name) == 0) {
            memmove(&nvtptr->nameval[i], &nvtptr->nameval[i+1], 
                   (nvtptr->size - i - 1) * sizeof(Nameval));
            nvtptr->size--;
            return 1;
        }
    }
    
    return 0;
}

// Демонстрація доступу до елементів через індекси (безпечно)
void access_by_index(NVtab *nvtptr) {
    std::cout << "Доступ за індексами:" << std::endl;
    for (int i = 0; i < nvtptr->size; i++) {
        std::cout << "Елемент [" << i << "]: " << nvtptr->nameval[i].name 
                  << " = " << nvtptr->nameval[i].value << std::endl;
    }
}

// Демонстрація доступу до елементів через вказівники (небезпечно)
void access_by_pointer(NVtab *nvtptr) {
    std::cout << "Доступ через вказівники:" << std::endl;
    Nameval *ptr = nvtptr->nameval;
    for (int i = 0; i < nvtptr->size; i++) {
        std::cout << "Елемент " << i << ": " << ptr->name 
                  << " = " << ptr->value << std::endl;
        ptr++; // переміщення вказівника до наступного елементу
    }
}

// Демонстрація проблеми з вказівниками при перерозподілі пам'яті
void demonstrate_pointer_problem(NVtab *nvtptr) {
    // Зберігаємо вказівник на перший елемент
    Nameval *first_element_ptr = &nvtptr->nameval[0];
    
    std::cout << "Адреса першого елемента до перерозподілу: " 
              << (void*)first_element_ptr << std::endl;
    
    // Додаємо багато елементів, щоб викликати перерозподіл пам'яті
    Nameval newitem;
    for (int i = 0; i < NVGROW + 5; i++) {
        char buffer[20];
        sprintf(buffer, "new_item%d", i);
        newitem.name = strdup(buffer);
        newitem.value = 100 + i;
        addname(nvtptr, newitem);
    }
    
    // Порівнюємо адресу першого елемента після перерозподілу
    std::cout << "Адреса першого елемента після перерозподілу: " 
              << (void*)&nvtptr->nameval[0] << std::endl;
    
    // Зверніть увагу, що якби ми використали first_element_ptr після перерозподілу,
    // це могло б призвести до невизначеної поведінки, оскільки пам'ять могла бути
    // переміщена в інше місце і перший вказівник більше не вказує на правильну адресу
    
    if (first_element_ptr != &nvtptr->nameval[0]) {
        std::cout << "УВАГА: Адреса змінилася! Використання вказівника first_element_ptr було б небезпечним!" << std::endl;
        
        // Наступний рядок може призвести до збою, тому він закоментований для безпеки
        // std::cout << "Небезпечний доступ: " << first_element_ptr->name << std::endl;
    }
}

int main() {
    NVtab nvtab;
    init_nvtab(&nvtab);
    
    Nameval item;
    
    // Додаємо кілька елементів
    item.name = strdup("item1");
    item.value = 1;
    addname(&nvtab, item);
    
    item.name = strdup("item2");
    item.value = 2;
    addname(&nvtab, item);
    
    item.name = strdup("item3");
    item.value = 3;
    addname(&nvtab, item);
    
    // Демонструємо доступ за індексами (безпечно)
    access_by_index(&nvtab);
    
    // Демонструємо доступ через вказівники (небезпечно)
    access_by_pointer(&nvtab);
    
    // Демонструємо проблему з вказівниками при перерозподілі пам'яті
    demonstrate_pointer_problem(&nvtab);
    
    // Звільнення пам'яті
    for (int i = 0; i < nvtab.size; i++) {
        free(nvtab.nameval[i].name);
    }
    free(nvtab.nameval);
    
    return 0;
}