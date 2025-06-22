#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
ЗАВДАННЯ 2: Аналіз функції delname та використання realloc

ПИТАННЯ:
1. Чи правильно не використовувати realloc в delname?
2. Чи можливо реалізувати задачі іншим шляхом?
3. Який варіант буде кращим?

ВІДПОВІДІ:

1. ТАК, правильно не використовувати realloc в delname з наступних причин:
   - Операція зменшення масиву через realloc є дорогою операцією
   - Ймовірно, що скоро знову буде потрібно додавати елементи
   - Фрагментація пам'яті може зростати при частих realloc
   - Продуктивність буде краща без зайвих перерозподілів

2. Альтернативні підходи:
   - Позначення елементів як "видалених" замість фізичного видалення
   - Використання порогового значення для зменшення масиву
   - Стратегія "ледачого" видалення з періодичною очисткою

3. Кращий варіант залежить від паттерну використання:
   - Для частих операцій додавання/видалення - не використовувати realloc
   - Для рідкісних видалень - можна використати realloc з порогом
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

// Оригінальна версія delname (без realloc)
int delname_original(struct NVtab *tab, char *name) {
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

// Альтернативна версія з realloc (для демонстрації)
int delname_with_realloc(struct NVtab *tab, char *name) {
    for (int i = 0; i < tab->nval; i++) {
        if (strcmp(tab->nameval[i].name, name) == 0) {
            memmove(tab->nameval + i, tab->nameval + (i + 1),
                   (tab->nval - (i + 1)) * sizeof(Nameval));
            tab->nval--;

            // Зменшуємо масив, якщо використовується менше половини
            if (tab->nval <= tab->max / 4 && tab->max > NVINIT) {
                int new_size = tab->max / 2;
                Nameval *nvp = (Nameval *) realloc(tab->nameval,
                                                 new_size * sizeof(Nameval));
                if (nvp != NULL) {
                    tab->nameval = nvp;
                    tab->max = new_size;
                    printf("Масив зменшено до %d елементів\n", new_size);
                }
            }
            return 1;
        }
    }
    return 0;
}

// Версія з позначенням як видалений
int delname_mark_deleted(struct NVtab *tab, char *name) {
    for (int i = 0; i < tab->nval; i++) {
        if (tab->nameval[i].name != NULL &&
            strcmp(tab->nameval[i].name, name) == 0) {
            // Позначаємо як видалений
            tab->nameval[i].name = NULL;
            printf("Елемент '%s' позначено як видалений\n", name);
            return 1;
        }
    }
    return 0;
}

int addname(struct NVtab *tab, Nameval newname) {
    Nameval *nvp;
    if (tab->nameval == NULL) {
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
    } else if (tab->nval >= tab->max) {
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

void print_array_info(struct NVtab *tab) {
    printf("Стан масиву: nval=%d, max=%d, використання=%.1f%%\n",
           tab->nval, tab->max,
           tab->max > 0 ? (double)tab->nval / tab->max * 100 : 0);
}

int main(void) {
    printf("=== Завдання 2: Аналіз функції delname ===\n\n");

    // Ініціалізація
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    // Створюємо багато елементів для демонстрації
    char names[][10] = {"Andy", "Billy", "Charlie", "David", "Eve",
                       "Frank", "Grace", "Henry", "Ivy", "Jack"};

    printf("1. Демонстрація оригінальної версії (без realloc):\n");
    for (int i = 0; i < 10; i++) {
        Nameval item = {names[i], i * 10};
        addname(&nvtab, item);
    }
    print_array_info(&nvtab);

    // Видаляємо кілька елементів
    delname_original(&nvtab, "Billy");
    delname_original(&nvtab, "David");
    delname_original(&nvtab, "Frank");
    delname_original(&nvtab, "Henry");
    delname_original(&nvtab, "Jack");

    printf("Після видалення 5 елементів:\n");
    print_array_info(&nvtab);
    printf("Зауваження: Пам'ять не звільнена, але готова для нових елементів\n\n");

    // Скидаємо для наступного тесту
    free(nvtab.nameval);
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.nameval = NULL;

    printf("2. Демонстрація версії з realloc:\n");
    for (int i = 0; i < 10; i++) {
        Nameval item = {names[i], i * 10};
        addname(&nvtab, item);
    }
    print_array_info(&nvtab);

    delname_with_realloc(&nvtab, "Billy");
    delname_with_realloc(&nvtab, "David");
    delname_with_realloc(&nvtab, "Frank");
    delname_with_realloc(&nvtab, "Henry");
    delname_with_realloc(&nvtab, "Jack");

    printf("Після видалення з realloc:\n");
    print_array_info(&nvtab);

    printf("\nВИСНОВОК:\n");
    printf("Оригінальний підхід (без realloc) є кращим для більшості випадків,\n");
    printf("оскільки зберігає продуктивність і готовність до нових додавань.\n");

    free(nvtab.nameval);
    return 0;
}