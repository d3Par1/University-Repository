#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
ЗАВДАННЯ 11: Модифікація функцій delitem та freeall
для звільнення пам'яті полів name

У оригінальному прикладі з лекції функції delitem та freeall
не звільняють пам'ять, виділену під поля name, що призводить
до витоку пам'яті.
*/

typedef struct Nameval Nameval;

struct Nameval {
    char *name;
    int value;
    Nameval *next;
};

// Глобальний вказівник на початок списку
Nameval *nvlist = NULL;

// ============= ДОПОМІЖНІ ФУНКЦІЇ =============

// Створення нового елемента з динамічним виділенням пам'яті для name
Nameval* newitem(char *name, int value) {
    Nameval *newp = (Nameval *) malloc(sizeof(Nameval));
    if (newp == NULL) {
        printf("ПОМИЛКА: Не вдалося виділити пам'ять для структури\n");
        return NULL;
    }

    // Виділяємо пам'ять для рядка name
    newp->name = (char *) malloc(strlen(name) + 1);
    if (newp->name == NULL) {
        printf("ПОМИЛКА: Не вдалося виділити пам'ять для name\n");
        free(newp);
        return NULL;
    }

    strcpy(newp->name, name);
    newp->value = value;
    newp->next = NULL;

    printf("Створено елемент: name='%s' (адреса %p), value=%d (структура %p)\n",
           name, (void*)newp->name, value, (void*)newp);

    return newp;
}

// Додавання на початок списку
Nameval* addfront(Nameval *listp, Nameval *newp) {
    newp->next = listp;
    return newp;
}

// Пошук елемента в списку
Nameval* lookup(Nameval *listp, char *name) {
    for (Nameval *p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0)
            return p;
    }
    return NULL;
}

// Виведення списку з адресами пам'яті
void print_list_with_addresses(Nameval *listp, const char *title) {
    printf("\n=== %s ===\n", title);
    if (listp == NULL) {
        printf("Список порожній\n");
        return;
    }

    int count = 0;
    for (Nameval *p = listp; p != NULL; p = p->next) {
        printf("[%d] Структура: %p\n", count, (void*)p);
        printf("    name: '%s' (адреса %p)\n", p->name, (void*)p->name);
        printf("    value: %d\n", p->value);
        printf("    next: %p\n", (void*)p->next);
        count++;
    }
    printf("Загальна кількість: %d елементів\n", count);
}

// ============= ОРИГІНАЛЬНІ ФУНКЦІЇ (З ВИТОКОМ ПАМ'ЯТІ) =============

// ОРИГІНАЛЬНА delitem (НЕ звільняє name) - для демонстрації проблеми
Nameval* delitem_original_buggy(Nameval *listp, char *name) {
    Nameval *p, *prev;

    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0) {
            if (prev == NULL) {
                listp = p->next;
            } else {
                prev->next = p->next;
            }

            printf("ОРИГІНАЛЬНА delitem: Звільняємо тільки структуру %p\n", (void*)p);
            printf("ВИТІК ПАМ'ЯТІ: name='%s' (адреса %p) НЕ звільнено!\n",
                   p->name, (void*)p->name);

            free(p);  // Звільняємо тільки структуру, name залишається в пам'яті!
            return listp;
        }
        prev = p;
    }
    return listp;
}

// ОРИГІНАЛЬНА freeall (НЕ звільняє name) - для демонстрації проблеми
void freeall_original_buggy(Nameval *listp) {
    Nameval *next;

    printf("\nОРИГІНАЛЬНА freeall (з витоком пам'яті):\n");
    for (Nameval *p = listp; p != NULL; p = next) {
        next = p->next;

        printf("Звільняємо структуру %p\n", (void*)p);
        printf("ВИТІК ПАМ'ЯТІ: name='%s' (адреса %p) НЕ звільнено!\n",
               p->name, (void*)p->name);

        free(p);  // Звільняємо тільки структуру!
    }
}

// ============= ВИПРАВЛЕНІ ФУНКЦІЇ =============

// ВИПРАВЛЕНА delitem (звільняє і name, і структуру)
Nameval* delitem_fixed(Nameval *listp, char *name) {
    Nameval *p, *prev;

    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(name, p->name) == 0) {
            if (prev == NULL) {
                listp = p->next;
            } else {
                prev->next = p->next;
            }

            printf("ВИПРАВЛЕНА delitem:\n");
            printf("  Звільняємо name='%s' (адреса %p)\n", p->name, (void*)p->name);
            free(p->name);  // ДОДАНО: звільняємо пам'ять для name

            printf("  Звільняємо структуру (адреса %p)\n", (void*)p);
            free(p);

            printf("  Пам'ять повністю звільнена!\n");
            return listp;
        }
        prev = p;
    }

    printf("Елемент '%s' не знайдено для видалення\n", name);
    return listp;
}

// ВИПРАВЛЕНА freeall (звільняє всю пам'ять)
void freeall_fixed(Nameval *listp) {
    Nameval *next;

    printf("\nВИПРАВЛЕНА freeall (без витоку пам'яті):\n");
    for (Nameval *p = listp; p != NULL; p = next) {
        next = p->next;

        printf("Звільняємо name='%s' (адреса %p)\n", p->name, (void*)p->name);
        free(p->name);  // ДОДАНО: звільняємо пам'ять для name

        printf("Звільняємо структуру (адреса %p)\n", (void*)p);
        free(p);

        printf("---\n");
    }
    printf("Вся пам'ять звільнена!\n");
}

// ============= ФУНКЦІЇ ДЛЯ АНАЛІЗУ ПАМ'ЯТІ =============

// Підрахунок використаної пам'яті
typedef struct {
    int structures_count;
    int strings_count;
    size_t structures_size;
    size_t strings_size;
    size_t total_size;
} MemoryUsage;

MemoryUsage calculate_memory_usage(Nameval *listp) {
    MemoryUsage usage = {0, 0, 0, 0, 0};

    for (Nameval *p = listp; p != NULL; p = p->next) {
        usage.structures_count++;
        usage.structures_size += sizeof(Nameval);

        if (p->name != NULL) {
            usage.strings_count++;
            usage.strings_size += strlen(p->name) + 1;  // +1 для '\0'
        }
    }

    usage.total_size = usage.structures_size + usage.strings_size;
    return usage;
}

void print_memory_usage(MemoryUsage usage, const char *title) {
    printf("\n=== %s ===\n", title);
    printf("Структури: %d штук, %zu байт\n",
           usage.structures_count, usage.structures_size);
    printf("Рядки:     %d штук, %zu байт\n",
           usage.strings_count, usage.strings_size);
    printf("Загалом:   %zu байт\n", usage.total_size);
}

// ============= ДЕМОНСТРАЦІЇ =============

void demonstrate_memory_leak() {
    printf("=== ДЕМОНСТРАЦІЯ ВИТОКУ ПАМ'ЯТІ ===\n");

    // Створюємо список
    Nameval *list = NULL;
    list = addfront(list, newitem("Charlie", 30));
    list = addfront(list, newitem("Bob", 20));
    list = addfront(list, newitem("Alice", 10));

    print_list_with_addresses(list, "Створений список");

    MemoryUsage before = calculate_memory_usage(list);
    print_memory_usage(before, "Використання пам'яті ДО видалення");

    // Видаляємо елемент оригінальною функцією (з витоком)
    printf("\n--- Видаляємо 'Bob' оригінальною функцією ---\n");
    list = delitem_original_buggy(list, "Bob");

    print_list_with_addresses(list, "Список після видалення");

    MemoryUsage after = calculate_memory_usage(list);
    print_memory_usage(after, "Використання пам'яті ПІСЛЯ видалення");

    printf("\nАНАЛІЗ ВИТОКУ:\n");
    printf("До видалення: %zu байт загалом\n", before.total_size);
    printf("Після видалення: %zu байт у списку\n", after.total_size);
    printf("Різниця: %zu байт\n", before.total_size - after.total_size);
    printf("Очікувана різниця: %zu байт (структура) + ~4 байти (рядок 'Bob')\n",
           sizeof(Nameval));
    printf("ФАКТИЧНО звільнено тільки %zu байт - є витік пам'яті!\n",
           sizeof(Nameval));

    // Звільняємо решту списку оригінальною функцією
    printf("\n--- Звільняємо решту списку оригінальною функцією ---\n");
    freeall_original_buggy(list);

    printf("\nРезультат: Пам'ять для всіх рядків name залишилася не звільненою!\n");
}

void demonstrate_proper_cleanup() {
    printf("\n\n=== ДЕМОНСТРАЦІЯ ПРАВИЛЬНОГО ЗВІЛЬНЕННЯ ===\n");

    // Створюємо новий список
    Nameval *list = NULL;
    list = addfront(list, newitem("Frank", 60));
    list = addfront(list, newitem("Eve", 50));
    list = addfront(list, newitem("David", 40));

    print_list_with_addresses(list, "Новий список для правильного тестування");

    MemoryUsage before = calculate_memory_usage(list);
    print_memory_usage(before, "Використання пам'яті ДО видалення");

    // Видаляємо елемент виправленою функцією
    printf("\n--- Видаляємо 'Eve' виправленою функцією ---\n");
    list = delitem_fixed(list, "Eve");

    print_list_with_addresses(list, "Список після правильного видалення");

    MemoryUsage after = calculate_memory_usage(list);
    print_memory_usage(after, "Використання пам'яті ПІСЛЯ видалення");

    printf("\nАНАЛІЗ ПРАВИЛЬНОГО ЗВІЛЬНЕННЯ:\n");
    printf("До видалення: %zu байт\n", before.total_size);
    printf("Після видалення: %zu байт\n", after.total_size);
    printf("Звільнено: %zu байт\n", before.total_size - after.total_size);
    printf("Пам'ять звільнена правильно!\n");

    // Звільняємо решту списку виправленою функцією
    printf("\n--- Звільняємо решту списку виправленою функцією ---\n");
    freeall_fixed(list);

    printf("\nРезультат: Вся пам'ять звільнена правильно!\n");
}

void analyze_memory_management() {
    printf("\n=== АНАЛІЗ УПРАВЛІННЯ ПАМ'ЯТТЮ ===\n");

    printf("ПРОБЛЕМА в оригінальних функціях:\n");
    printf("• delitem() звільняє тільки free(p), але не free(p->name)\n");
    printf("• freeall() звільняє тільки free(p), але не free(p->name)\n");
    printf("• Результат: витік пам'яті для всіх рядків name\n\n");

    printf("РІШЕННЯ:\n");
    printf("• Перед free(p) додати free(p->name)\n");
    printf("• Перевіряти p->name != NULL (якщо може бути NULL)\n");
    printf("• Правильний порядок: спочатку name, потім структура\n\n");

    printf("ПРАВИЛО: 'Хто виділив - той і звільняє'\n");
    printf("• newitem() виділяє пам'ять для name → delitem()/freeall() повинні її звільнити\n");
    printf("• newitem() виділяє пам'ять для структури → delitem()/freeall() повинні її звільнити\n\n");

    printf("НАСЛІДКИ ВИТОКУ ПАМ'ЯТІ:\n");
    printf("• Зростання використання пам'яті\n");
    printf("• Можлива нестача пам'яті в системі\n");
    printf("• Погіршення продуктивності\n");
    printf("• Потенційний crash програми\n\n");

    printf("ІНСТРУМЕНТИ ДЛЯ ВИЯВЛЕННЯ:\n");
    printf("• valgrind (Linux/Mac)\n");
    printf("• AddressSanitizer (GCC/Clang)\n");
    printf("• Static Analysis Tools\n");
    printf("• Memory Profilers\n");
}

void demonstrate_safe_patterns() {
    printf("\n=== БЕЗПЕЧНІ ПАТТЕРНИ РОБОТИ З ПАМ'ЯТТЮ ===\n");

    printf("1. СТВОРЕННЯ:\n");
    printf("   ptr = malloc(size);\n");
    printf("   if (ptr == NULL) { /* обробка помилки */ }\n\n");

    printf("2. ЗВІЛЬНЕННЯ:\n");
    printf("   if (ptr != NULL) {\n");
    printf("       free(ptr);\n");
    printf("       ptr = NULL;  // запобігає повторному звільненню\n");
    printf("   }\n\n");

    printf("3. ДЛЯ СТРУКТУР З УКАЗІВНИКАМИ:\n");
    printf("   // Звільняємо спочатку вкладені вказівники\n");
    printf("   free(item->name);\n");
    printf("   free(item->data);\n");
    printf("   // Потім саму структуру\n");
    printf("   free(item);\n\n");

    printf("4. ПЕРЕВІРКИ:\n");
    printf("   • Завжди перевіряти результат malloc()\n");
    printf("   • Ніколи не звільняти NULL\n");
    printf("   • Не використовувати вказівники після free()\n");
    printf("   • Не звільняти пам'ять двічі\n");
}

int main(void) {
    printf("=== Завдання 11: Виправлення витоку пам'яті ===\n");

    demonstrate_memory_leak();
    demonstrate_proper_cleanup();
    analyze_memory_management();
    demonstrate_safe_patterns();

    printf("\n=== ВИСНОВКИ ===\n");
    printf("Виправлені функції:\n");
    printf("1. delitem_fixed() - звільняє і name, і структуру\n");
    printf("2. freeall_fixed() - звільняє всю виділену пам'ять\n\n");

    printf("Ключові зміни:\n");
    printf("• Додано free(p->name) перед free(p)\n");
    printf("• Зберігається правильний порядок звільнення\n");
    printf("• Додано інформативні повідомлення для відстеження\n\n");

    printf("Практичне значення:\n");
    printf("• Попередження витоків пам'яті\n");
    printf("• Стабільність програми\n");
    printf("• Ефективне використання ресурсів\n");
    printf("• Відповідність стандартам якості коду\n");

    return 0;
}