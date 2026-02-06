#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
};

struct NVtab {
    int nval;         /* current number of values */
    int max;          /* allocated number of values */
    Nameval *nameval; /* array of name-value pairs */
} nvtab;

enum {NVINIT = 1, NVGROW = 2};

/* addname: add new name and value to nvtab */
int addname(struct NVtab *tab, Nameval newname) {
    Nameval *nvp;
    if (tab->nameval == NULL) { /* first time */
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
    } else if (tab->nval >= tab->max) { /* grow */
        nvp = (Nameval *) realloc(tab->nameval,
                                (NVGROW * tab->max) * sizeof(Nameval));
        if (nvp == NULL)
            return -1;
        tab->max *= NVGROW;
        tab->nameval = nvp;
        printf("Масив розширено до %d елементів\n", tab->max);
    }
    tab->nameval[tab->nval] = newname;
    return tab->nval++;
}

/* delname: remove first matching nameval from nvtab */
int delname(struct NVtab *tab, char *name) {
    for (int i = 0; i < tab->nval; i++) {
        if (strcmp(tab->nameval[i].name, name) == 0) {
            memmove(tab->nameval + i, tab->nameval + (i + 1),
                   (tab->nval - (i + 1)) * sizeof(Nameval));
            tab->nval--;
            return 1;
        }
    }
    return 0;
}

int main(void) {
    printf("=== Завдання 1: Модифіковані функції addname та delname ===\n");

    // Ініціалізація глобальної змінної
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Тестування
    Nameval item1 = {"Andy", 12};
    Nameval item2 = {"Billy", 18};
    Nameval item3 = {"Charlie", 25};

    printf("Додаємо елементи:\n");
    addname(&nvtab, item1);
    printf("Додано: %s = %d\n", item1.name, item1.value);

    addname(&nvtab, item2);
    printf("Додано: %s = %d\n", item2.name, item2.value);

    addname(&nvtab, item3);
    printf("Додано: %s = %d\n", item3.name, item3.value);

    printf("\nВсі елементи:\n");
    for (int i = 0; i < nvtab.nval; i++) {
        printf("%s = %d\n", nvtab.nameval[i].name, nvtab.nameval[i].value);
    }

    printf("\nВидаляємо 'Billy':\n");
    if (delname(&nvtab, "Billy")) {
        printf("Елемент 'Billy' видалено\n");
    }

    printf("\nЕлементи після видалення:\n");
    for (int i = 0; i < nvtab.nval; i++) {
        printf("%s = %d\n", nvtab.nameval[i].name, nvtab.nameval[i].value);
    }

    // Звільнення пам'яті
    free(nvtab.nameval);

    return 0;
}