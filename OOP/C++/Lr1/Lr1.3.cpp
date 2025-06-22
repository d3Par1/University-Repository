#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
ЗАВДАННЯ 3: Чому звертатися до елементів слід по індексам?

ВІДПОВІДЬ:
При перерозподілі пам'яті (realloc) адреса масиву може змінитися!
Якщо ми зберігаємо вказівники на елементи масиву, то після realloc
ці вказівники стануть недійсними і будуть вказувати на звільнену пам'ять.

Індекси завжди залишаються дійсними, оскільки вони відносні від початку масиву.
*/

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
};

struct NVtab {
    int nval;
    int max;
    Nameval *nameval;
} nvtab;

enum {NVINIT = 1, NVGROW = 2};

int addname(struct NVtab *tab, Nameval newname) {
    Nameval *nvp;
    if (tab->nameval == NULL) {
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
    } else if (tab->nval >= tab->max) {
        printf("Перерозподіл пам'яті! Старий адрес: %p\n", (void*)tab->nameval);
        nvp = (Nameval *) realloc(tab->nameval,
                                (NVGROW * tab->max) * sizeof(Nameval));
        if (nvp == NULL)
            return -1;
        tab->max *= NVGROW;
        tab->nameval = nvp;
        printf("Новий адрес: %p\n", (void*)tab->nameval);
        printf("Адреса змінилася: %s\n", (nvp != tab->nameval) ? "ТАК" : "НІ");
    }
    tab->nameval[tab->nval] = newname;
    return tab->nval++;
}

// Демонстрація НЕБЕЗПЕЧНОГО використання вказівників
void demonstrate_pointer_danger() {
    printf("=== ДЕМОНСТРАЦІЯ НЕБЕЗПЕКИ ВКАЗІВНИКІВ ===\n");

    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Додаємо перший елемент
    Nameval item1 = {"Andy", 12};
    addname(&nvtab, item1);

    // НЕБЕЗПЕКА: зберігаємо вказівник на елемент
    Nameval *dangerous_ptr = &nvtab.nameval[0];
    printf("\nВказівник на перший елемент: %p\n", (void*)dangerous_ptr);
    printf("Значення через вказівник: %s = %d\n",
           dangerous_ptr->name, dangerous_ptr->value);

    // Додаємо більше елементів, що призведе до realloc
    Nameval item2 = {"Billy", 18};
    printf("\nДодаємо другий елемент (спричинить realloc):\n");
    addname(&nvtab, item2);

    printf("\nПеревіряємо вказівник після realloc:\n");
    printf("Старий вказівник все ще вказує на: %p\n", (void*)dangerous_ptr);
    printf("Новий адрес першого елемента: %p\n", (void*)&nvtab.nameval[0]);

    if (dangerous_ptr != &nvtab.nameval[0]) {
        printf("УВАГА! Вказівник став недійсним!\n");
        printf("Спроба доступу через старий вказівник може призвести до краху!\n");
    }

    // Безпечний доступ через індекс
    printf("\nБезпечний доступ через індекс [0]:\n");
    printf("Значення: %s = %d\n", nvtab.nameval[0].name, nvtab.nameval[0].value);
}

// Демонстрація ПРАВИЛЬНОГО використання індексів
void demonstrate_safe_indexing() {
    printf("\n=== ДЕМОНСТРАЦІЯ БЕЗПЕЧНИХ ІНДЕКСІВ ===\n");

    // Скидаємо масив
    free(nvtab.nameval);
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Зберігаємо індекси замість вказівників
    int first_element_index = 0;

    Nameval item1 = {"Charlie", 25};
    addname(&nvtab, item1);
    printf("Індекс першого елемента: %d\n", first_element_index);
    printf("Адрес масиву: %p\n", (void*)nvtab.nameval);

    // Додаємо більше елементів
    for (int i = 0; i < 5; i++) {
        char name[20];
        sprintf(name, "Person%d", i);
        Nameval item = {strdup(name), i * 10};
        printf("\nДодаємо елемент %d:\n", i);
        addname(&nvtab, item);
        printf("Адрес масиву: %p\n", (void*)nvtab.nameval);
    }

    printf("\nДоступ до першого елемента через збережений індекс:\n");
    printf("nvtab.nameval[%d] = %s, %d\n",
           first_element_index,
           nvtab.nameval[first_element_index].name,
           nvtab.nameval[first_element_index].value);

    printf("\nІндекс завжди працює правильно, незалежно від realloc!\n");

    // Звільняємо динамічну пам'ять для імен
    for (int i = 1; i < nvtab.nval; i++) {
        free(nvtab.nameval[i].name);
    }
}

void demonstrate_multiple_pointers() {
    printf("\n=== ДЕМОНСТРАЦІЯ З КІЛЬКОМА ВКАЗІВНИКАМИ ===\n");

    free(nvtab.nameval);
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Створюємо кілька елементів
    Nameval items[] = {
        {"Alice", 100},
        {"Bob", 200},
        {"Carol", 300}
    };

    for (int i = 0; i < 3; i++) {
        addname(&nvtab, items[i]);
    }

    // Зберігаємо вказівники на всі елементи (НЕБЕЗПЕЧНО!)
    Nameval *ptrs[3];
    for (int i = 0; i < 3; i++) {
        ptrs[i] = &nvtab.nameval[i];
        printf("Вказівник %d: %p -> %s\n", i, (void*)ptrs[i], ptrs[i]->name);
    }

    // Форсуємо realloc додаванням елементів
    printf("\nФорсуємо realloc додаванням нових елементів:\n");
    for (int i = 3; i < 10; i++) {
        Nameval item = {"NewPerson", i * 10};
        addname(&nvtab, item);
    }

    printf("\nПеревіряємо старі вказівники:\n");
    for (int i = 0; i < 3; i++) {
        printf("Старий вказівник %d: %p\n", i, (void*)ptrs[i]);
        printf("Новий адрес елемента %d: %p\n", i, (void*)&nvtab.nameval[i]);
        printf("Співпадають: %s\n", (ptrs[i] == &nvtab.nameval[i]) ? "ТАК" : "НІ");
    }

    printf("\nВИСНОВОК: Всі старі вказівники стали недійсними!\n");
}

int main(void) {
    printf("=== Завдання 3: Індекси vs Вказівники ===\n\n");

    demonstrate_pointer_danger();
    demonstrate_safe_indexing();
    demonstrate_multiple_pointers();

    printf("\n=== ЗАГАЛЬНИЙ ВИСНОВОК ===\n");
    printf("При роботі з динамічними масивами, які можуть змінювати розмір:\n");
    printf("1. Використовуйте ІНДЕКСИ для доступу до елементів\n");
    printf("2. НЕ зберігайте вказівники на елементи масиву\n");
    printf("3. Після кожного potencial realloc оновлюйте всі вказівники\n");
    printf("4. Індекси залишаються дійсними незалежно від realloc\n");

    free(nvtab.nameval);
    return 0;
}