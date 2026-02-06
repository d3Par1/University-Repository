#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Nameval Nameval;

struct Nameval {
    char *name;  // NULL означає "видалений елемент"
    int value;
};

struct NVtab {
    int nval;        // загальна кількість слотів (включаючи видалені)
    int max;         // виділена кількість
    int active;      // кількість активних (не видалених) елементів
    Nameval *nameval;
} nvtab;

enum {NVINIT = 1, NVGROW = 2};

// Знаходить перший вільний слот або повертає -1
int find_free_slot(struct NVtab *tab) {
    for (int i = 0; i < tab->nval; i++) {
        if (tab->nameval[i].name == NULL) {
            return i;
        }
    }
    return -1; // немає вільних слотів
}

// Модифікована функція addname
int addname_with_reuse(struct NVtab *tab, Nameval newname) {
    Nameval *nvp;

    if (tab->nameval == NULL) {
        // Перша ініціалізація
        tab->nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        if (tab->nameval == NULL)
            return -1;
        tab->max = NVINIT;
        tab->nval = 0;
        tab->active = 0;
    }

    // Спочатку шукаємо вільний слот серед видалених
    int free_slot = find_free_slot(tab);
    if (free_slot != -1) {
        printf("Повторно використовуємо слот %d\n", free_slot);
        tab->nameval[free_slot] = newname;
        tab->active++;
        return free_slot;
    }

    // Якщо вільних слотів немає, перевіряємо чи потрібно розширити масив
    if (tab->nval >= tab->max) {
        printf("Розширюємо масив з %d до %d\n", tab->max, NVGROW * tab->max);
        nvp = (Nameval *) realloc(tab->nameval,
                                (NVGROW * tab->max) * sizeof(Nameval));
        if (nvp == NULL)
            return -1;
        tab->max *= NVGROW;
        tab->nameval = nvp;
    }

    // Додаємо в кінець
    tab->nameval[tab->nval] = newname;
    tab->active++;
    return tab->nval++;
}

// Модифікована функція delname (позначає як видалений)
int delname_mark_deleted(struct NVtab *tab, char *name) {
    for (int i = 0; i < tab->nval; i++) {
        if (tab->nameval[i].name != NULL &&
            strcmp(tab->nameval[i].name, name) == 0) {

            printf("Позначаємо '%s' в слоті %d як видалений\n", name, i);
            tab->nameval[i].name = NULL;  // Позначаємо як видалений
            tab->nameval[i].value = 0;    // Очищаємо значення
            tab->active--;
            return 1;
        }
    }
    return 0;
}

// Функція для компактування масиву (видалення помічених елементів)
void compact_array(struct NVtab *tab) {
    printf("\n=== Компактування масиву ===\n");
    printf("До компактування: nval=%d, active=%d\n", tab->nval, tab->active);

    int write_pos = 0;
    for (int read_pos = 0; read_pos < tab->nval; read_pos++) {
        if (tab->nameval[read_pos].name != NULL) {
            if (write_pos != read_pos) {
                printf("Переміщуємо елемент з позиції %d на позицію %d\n",
                       read_pos, write_pos);
                tab->nameval[write_pos] = tab->nameval[read_pos];
            }
            write_pos++;
        }
    }

    tab->nval = tab->active;
    printf("Після компактування: nval=%d, active=%d\n", tab->nval, tab->active);

    // Опціонально: зменшити розмір масиву
    if (tab->nval <= tab->max / 4 && tab->max > NVINIT) {
        int new_max = tab->max / 2;
        Nameval *nvp = (Nameval *) realloc(tab->nameval,
                                         new_max * sizeof(Nameval));
        if (nvp != NULL) {
            tab->nameval = nvp;
            tab->max = new_max;
            printf("Розмір масиву зменшено до %d\n", new_max);
        }
    }
}

// Функція для підрахунку фрагментації
double calculate_fragmentation(struct NVtab *tab) {
    if (tab->nval == 0) return 0.0;
    return (double)(tab->nval - tab->active) / tab->nval * 100.0;
}

void print_array_detailed(struct NVtab *tab, const char *title) {
    printf("\n%s:\n", title);
    printf("Статистика: nval=%d, active=%d, max=%d, фрагментація=%.1f%%\n",
           tab->nval, tab->active, tab->max, calculate_fragmentation(tab));

    for (int i = 0; i < tab->nval; i++) {
        if (tab->nameval[i].name != NULL) {
            printf("  [%d] %s = %d (активний)\n",
                   i, tab->nameval[i].name, tab->nameval[i].value);
        } else {
            printf("  [%d] <видалений> (вільний слот)\n", i);
        }
    }
}

void print_active_only(struct NVtab *tab, const char *title) {
    printf("\n%s (тільки активні):\n", title);
    for (int i = 0; i < tab->nval; i++) {
        if (tab->nameval[i].name != NULL) {
            printf("  %s = %d\n", tab->nameval[i].name, tab->nameval[i].value);
        }
    }
}

void demonstrate_lazy_deletion() {
    printf("=== ДЕМОНСТРАЦІЯ ЛЕДАЧОГО ВИДАЛЕННЯ ===\n");

    // Ініціалізація
    nvtab.nval = 0;
    nvtab.max = 0;
    nvtab.active = 0;
    nvtab.nameval = NULL;

    // Додаємо початкові дані
    char names[][10] = {"Alice", "Bob", "Charlie", "David", "Eve"};
    int values[] = {10, 20, 30, 40, 50};

    printf("Додаємо початкові елементи:\n");
    for (int i = 0; i < 5; i++) {
        Nameval item = {names[i], values[i]};
        int pos = addname_with_reuse(&nvtab, item);
        printf("  %s додано в позицію %d\n", names[i], pos);
    }

    print_array_detailed(&nvtab, "Початковий стан");

    // Видаляємо кілька елементів
    printf("\n--- Видаляємо елементи ---\n");
    delname_mark_deleted(&nvtab, "Bob");
    delname_mark_deleted(&nvtab, "David");

    print_array_detailed(&nvtab, "Після видалення Bob та David");

    // Додаємо нові елементи (повинні зайняти вільні слоти)
    printf("\n--- Додаємо нові елементи ---\n");
    Nameval frank = {"Frank", 60};
    Nameval grace = {"Grace", 70};

    int pos1 = addname_with_reuse(&nvtab, frank);
    printf("  Frank додано в позицію %d\n", pos1);

    int pos2 = addname_with_reuse(&nvtab, grace);
    printf("  Grace додано в позицію %d\n", pos2);

    print_array_detailed(&nvtab, "Після додавання Frank та Grace");
    print_active_only(&nvtab, "Активні елементи");
}

void demonstrate_fragmentation_management() {
    printf("\n=== УПРАВЛІННЯ ФРАГМЕНТАЦІЄЮ ===\n");

    // Додаємо ще елементів для демонстрації
    char more_names[][10] = {"Henry", "Ivy", "Jack", "Kate"};
    for (int i = 0; i < 4; i++) {
        Nameval item = {more_names[i], (i + 8) * 10};
        addname_with_reuse(&nvtab, item);
    }

    print_array_detailed(&nvtab, "Перед створенням фрагментації");

    // Створюємо фрагментацію
    printf("\n--- Створюємо фрагментацію ---\n");
    delname_mark_deleted(&nvtab, "Alice");
    delname_mark_deleted(&nvtab, "Charlie");
    delname_mark_deleted(&nvtab, "Henry");
    delname_mark_deleted(&nvtab, "Jack");

    print_array_detailed(&nvtab, "Після створення фрагментації");

    printf("Фрагментація: %.1f%% - час для компактування!\n",
           calculate_fragmentation(&nvtab));

    // Компактуємо масив
    compact_array(&nvtab);
    print_array_detailed(&nvtab, "Після компактування");
}

void demonstrate_isolation() {
    printf("\n=== ІЗОЛЯЦІЯ ВІД ЗМІН ===\n");

    printf("Питання: Наскільки інша частина програми ізольована від змін?\n\n");

    printf("ВІДПОВІДЬ:\n");
    printf("Інші частини програми ДОБРЕ ІЗОЛЬОВАНІ, оскільки:\n\n");

    printf("1. Інтерфейс функцій не змінився:\n");
    printf("   - addname() працює так само\n");
    printf("   - delname() працює так само\n");
    printf("   - Повертають ті ж значення\n\n");

    printf("2. Зовнішній код не знає про внутрішню реалізацію:\n");
    printf("   - Не знає про поле 'active'\n");
    printf("   - Не знає про NULL-значення в name\n");
    printf("   - Не знає про стратегію повторного використання\n\n");

    printf("3. Поведінка залишається передбачуваною:\n");
    printf("   - Додані елементи доступні\n");
    printf("   - Видалені елементи недоступні\n");
    printf("   - Порядок може змінюватися, але це не критично\n\n");

    printf("4. Прозорість змін:\n");
    printf("   - Код, що використовує масив, не потребує змін\n");
    printf("   - Можна додати функцію компактування опціонально\n");
    printf("   - Можна додати статистику без зміни основного API\n");

    // Демонстрація використання зовнішнім кодом
    printf("\nПриклад зовнішнього коду (не знає про внутрішню структуру):\n");

    print_active_only(&nvtab, "Список активних користувачів");

    printf("\nЗовнішній код бачить тільки активні елементи і працює нормально!\n");
}

int main(void) {
    printf("=== Завдання 6: Позначення як невикористовуваних ===\n\n");

    demonstrate_lazy_deletion();
    demonstrate_fragmentation_management();
    demonstrate_isolation();


    free(nvtab.nameval);
    return 0;
}