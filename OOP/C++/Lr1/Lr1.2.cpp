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

// Функція для ініціалізації масиву
void init_nvtab(NVtab *nvtptr) {
    nvtptr->nameval = NULL;
    nvtptr->size = 0;
    nvtptr->max = 0;
}

// Стандартна функція addname
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

// Стандартна функція delname (не використовує realloc для зменшення масиву)
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

// Покращена функція delname, яка використовує realloc для зменшення масиву
int delname_improved(NVtab *nvtptr, char *name) {
    int i;
    Nameval *newval;
    
    for (i = 0; i < nvtptr->size; i++) {
        if (strcmp(nvtptr->nameval[i].name, name) == 0) {
            // Спочатку зсуваємо елементи
            memmove(&nvtptr->nameval[i], &nvtptr->nameval[i+1], 
                   (nvtptr->size - i - 1) * sizeof(Nameval));
            nvtptr->size--;
            
            // Перевіряємо, чи можна зменшити масив
            if (nvtptr->size > 0 && nvtptr->max - nvtptr->size >= NVGROW * 2) {
                newval = (Nameval *) realloc(nvtptr->nameval, 
                                          (nvtptr->max - NVGROW) * sizeof(Nameval));
                if (newval != NULL) {
                    nvtptr->nameval = newval;
                    nvtptr->max -= NVGROW;
                }
                // Якщо realloc не вдалося, ми просто продовжуємо з поточним масивом
            }
            
            return 1;
        }
    }
    
    return 0;
}

// Альтернативна реалізація з використанням списку замість масиву
typedef struct NamevalNode {
    Nameval data;
    struct NamevalNode *next;
} NamevalNode;

typedef struct {
    NamevalNode *head;
    int size;
} NamevalList;

// Ініціалізація списку
void init_list(NamevalList *list) {
    list->head = NULL;
    list->size = 0;
}

// Додавання елемента до списку
void add_to_list(NamevalList *list, Nameval newname) {
    NamevalNode *newnode = (NamevalNode *)malloc(sizeof(NamevalNode));
    newnode->data = newname;
    newnode->next = list->head;
    list->head = newnode;
    list->size++;
}

// Видалення елемента зі списку
int delete_from_list(NamevalList *list, char *name) {
    NamevalNode *current = list->head;
    NamevalNode *prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->data.name, name) == 0) {
            if (prev == NULL) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            list->size--;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    
    return 0;
}

// Звільнення пам'яті списку
void free_list(NamevalList *list) {
    NamevalNode *current = list->head;
    NamevalNode *next;
    
    while (current != NULL) {
        next = current->next;
        free(current->data.name);
        free(current);
        current = next;
    }
    
    list->head = NULL;
    list->size = 0;
}

int main() {
    // Демонстрація використання різних підходів
    NVtab nvtab;
    init_nvtab(&nvtab);
    
    Nameval item;
    
    // Додаємо елементи до масиву
    for (int i = 0; i < 25; i++) {
        char buf[20];
        sprintf(buf, "item%d", i);
        item.name = strdup(buf);
        item.value = i;
        addname(&nvtab, item);
    }
    
    std::cout << "Початковий розмір масиву: " << nvtab.size << ", макс: " << nvtab.max << std::endl;
    
    // Видаляємо кілька елементів
    delname(&nvtab, (char*)"item5");
    delname(&nvtab, (char*)"item10");
    delname(&nvtab, (char*)"item15");
    
    std::cout << "Після видалення (стандартний delname): " << nvtab.size << ", макс: " << nvtab.max << std::endl;
    
    // Видаляємо ще кілька елементів з покращеною функцією
    delname_improved(&nvtab, (char*)"item1");
    delname_improved(&nvtab, (char*)"item2");
    delname_improved(&nvtab, (char*)"item3");
    delname_improved(&nvtab, (char*)"item4");
    delname_improved(&nvtab, (char*)"item6");
    delname_improved(&nvtab, (char*)"item7");
    delname_improved(&nvtab, (char*)"item8");
    delname_improved(&nvtab, (char*)"item9");
    
    std::cout << "Після видалення (покращений delname): " << nvtab.size << ", макс: " << nvtab.max << std::endl;
    
    // Демонстрація використання списку
    NamevalList list;
    init_list(&list);
    
    for (int i = 0; i < 10; i++) {
        char buf[20];
        sprintf(buf, "list_item%d", i);
        item.name = strdup(buf);
        item.value = i * 10;
        add_to_list(&list, item);
    }
    
    std::cout << "Розмір списку: " << list.size << std::endl;
    
    delete_from_list(&list, (char*)"list_item3");
    delete_from_list(&list, (char*)"list_item7");
    
    std::cout << "Розмір списку після видалення: " << list.size << std::endl;
    
    // Звільнення пам'яті
    for (int i = 0; i < nvtab.size; i++) {
        free(nvtab.nameval[i].name);
    }
    free(nvtab.nameval);
    
    free_list(&list);
    
    return 0;
}