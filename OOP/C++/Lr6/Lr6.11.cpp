#include <iostream>
#include <cstring>
using namespace std;

// Версія 1: Використання конструктора копій
class strtype_copy {
    char *p;
    int len;
    
public:
    strtype_copy(const char *ptr);
    strtype_copy(const strtype_copy &obj);  // Конструктор копій
    ~strtype_copy() { delete[] p; }
    
    char *get() { return p; }
    strtype_copy& operator=(const strtype_copy &obj);
    
    // Функція, що приймає об'єкт за значенням
    void process_by_value(strtype_copy obj) {
        cout << "Обробка копії: " << obj.get() << "\n";
    }
    
    // Функція, що повертає об'єкт за значенням
    strtype_copy create_copy() {
        strtype_copy temp("Створено копію");
        return temp;  // Викликається конструктор копій
    }
};

strtype_copy::strtype_copy(const char *ptr) {
    len = strlen(ptr);
    p = new char[len + 1];
    strcpy(p, ptr);
    cout << "Створено strtype_copy: \"" << p << "\" (конструктор)\n";
}

strtype_copy::strtype_copy(const strtype_copy &obj) {
    len = obj.len;
    p = new char[len + 1];
    strcpy(p, obj.p);
    cout << "Створено strtype_copy: \"" << p << "\" (конструктор копій)\n";
}

strtype_copy& strtype_copy::operator=(const strtype_copy &obj) {
    if(this == &obj) return *this;
    
    delete[] p;
    len = obj.len;
    p = new char[len + 1];
    strcpy(p, obj.p);
    cout << "Присвоєння strtype_copy: \"" << p << "\"\n";
    return *this;
}

// Версія 2: Використання посилань
class strtype_ref {
    char *p;
    int len;
    
public:
    strtype_ref(const char *ptr);
    strtype_ref(const strtype_ref &obj);  // Конструктор копій (все одно потрібен)
    ~strtype_ref() { delete[] p; }
    
    char *get() { return p; }
    strtype_ref& operator=(const strtype_ref &obj);
    
    // Функція, що приймає об'єкт за посиланням
    void process_by_reference(const strtype_ref &obj) {
        cout << "Обробка посилання: " << obj.p << "\n";
    }
    
    // Функція, що повертає посилання
    strtype_ref& get_reference() {
        cout << "Повернення посилання на існуючий об'єкт\n";
        return *this;
    }
};

strtype_ref::strtype_ref(const char *ptr) {
    len = strlen(ptr);
    p = new char[len + 1];
    strcpy(p, ptr);
    cout << "Створено strtype_ref: \"" << p << "\" (конструктор)\n";
}

strtype_ref::strtype_ref(const strtype_ref &obj) {
    len = obj.len;
    p = new char[len + 1];
    strcpy(p, obj.p);
    cout << "Створено strtype_ref: \"" << p << "\" (конструктор копій)\n";
}

strtype_ref& strtype_ref::operator=(const strtype_ref &obj) {
    if(this == &obj) return *this;
    
    delete[] p;
    len = obj.len;
    p = new char[len + 1];
    strcpy(p, obj.p);
    cout << "Присвоєння strtype_ref: \"" << p << "\"\n";
    return *this;
}

// Демонстраційні функції
void demo_copy_constructor() {
    cout << "\n=== ДЕМОНСТРАЦІЯ ВЕРСІЇ З КОНСТРУКТОРОМ КОПІЙ ===\n";
    
    strtype_copy str1("Оригінальний рядок");
    
    cout << "\n1. Передача за значенням:\n";
    str1.process_by_value(str1);  // Створюється копія
    
    cout << "\n2. Повернення за значенням:\n";
    strtype_copy str2 = str1.create_copy();  // Копіювання при поверненні
    
    cout << "\n3. Ініціалізація копією:\n";
    strtype_copy str3 = str1;  // Конструктор копій
    
    cout << "\n4. Присвоєння:\n";
    strtype_copy str4("Тимчасовий");
    str4 = str1;  // Оператор присвоєння
    
    cout << "\nЗавершення demo_copy_constructor...\n";
}

void demo_references() {
    cout << "\n=== ДЕМОНСТРАЦІЯ ВЕРСІЇ З ПОСИЛАННЯМИ ===\n";
    
    strtype_ref str1("Оригінальний рядок");
    
    cout << "\n1. Передача за посиланням:\n";
    str1.process_by_reference(str1);  // Копія НЕ створюється
    
    cout << "\n2. Повернення посилання:\n";
    strtype_ref &str_ref = str1.get_reference();  // Копія НЕ створюється
    cout << "Отримано посилання на: " << str_ref.get() << "\n";
    
    cout << "\n3. Ініціалізація посиланням:\n";
    strtype_ref &str2 = str1;  // Псевдонім, копія НЕ створюється
    cout << "Посилання str2 вказує на: " << str2.get() << "\n";
    
    cout << "\n4. Присвоєння (все одно викликається):\n";
    strtype_ref str3("Тимчасовий");
    str3 = str1;  // Оператор присвоєння
    
    cout << "\nЗавершення demo_references...\n";
}

// Ситуація, де конструктор копій краще
class DataProcessor {
    strtype_copy data;
    
public:
    DataProcessor(const char *initial_data) : data(initial_data) {}
    
    // Метод, що повертає модифіковану копію без зміни оригіналу
    strtype_copy get_processed_copy() {
        strtype_copy temp = data;  // Створюємо копію
        // Тут могла б бути обробка temp
        cout << "Повертаємо оброблену копію: " << temp.get() << "\n";
        return temp;  // Повертаємо копію
    }
    
    strtype_copy& get_original() {
        return data;  // Повертаємо посилання на оригінал
    }
};

void demo_when_copy_is_better() {
    cout << "\n=== КОЛИ КОНСТРУКТОР КОПІЙ КРАЩИЙ ===\n";
    
    DataProcessor processor("Важливі дані");
    
    cout << "\n1. Отримання копії для безпечної обробки:\n";
    strtype_copy safe_copy = processor.get_processed_copy();
    
    cout << "\n2. Модифікація копії не впливає на оригінал:\n";
    cout << "Оригінал: " << processor.get_original().get() << "\n";
    cout << "Копія: " << safe_copy.get() << "\n";
    
    cout << "\n3. Випадок, коли потрібно зберегти стан:\n";
    strtype_copy backup = processor.get_original();  // Створюємо backup
    
    // Модифікуємо оригінал
    processor.get_original() = strtype_copy("Змінені дані");
    
    cout << "Після модифікації:\n";
    cout << "Поточні дані: " << processor.get_original().get() << "\n";
    cout << "Backup дані: " << backup.get() << "\n";
}

int main() {
    cout << "ПОРІВНЯННЯ КОНСТРУКТОРА КОПІЙ ТА ПОСИЛАНЬ:\n";
    cout << "==========================================\n";
    
    // Демонстрація обох підходів
    demo_copy_constructor();
    demo_references();
    demo_when_copy_is_better();
    
    cout << "\n\nАНАЛІЗ ПЕРЕВАГ ТА НЕДОЛІКІВ:\n";
    cout << "============================\n\n";
    
    cout << "КОНСТРУКТОР КОПІЙ:\n";
    cout << "Переваги:\n";
    cout << "+ Безпека: кожен об'єкт має власну копію даних\n";
    cout << "+ Незалежність: зміни в одній копії не впливають на інші\n";
    cout << "+ Простота використання: можна передавати за значенням\n";
    cout << "+ Можливість створення backup'ів та снімків стану\n";
    cout << "\nНедоліки:\n";
    cout << "- Додаткові витрати пам'яті на копії\n";
    cout << "- Додаткові витрати часу на копіювання\n";
    cout << "- Може бути неефективним для великих об'єктів\n\n";
    
    cout << "ПОСИЛАННЯ:\n";
    cout << "Переваги:\n";
    cout << "+ Ефективність: немає додаткового копіювання\n";
    cout << "+ Економія пам'яті: працюємо з оригінальними даними\n";
    cout << "+ Швидкість: немає накладних витрат на створення копій\n";
    cout << "+ Можливість модифікації оригінального об'єкта\n";
    cout << "\nНедоліки:\n";
    cout << "- Менша безпека: зміни впливають на оригінал\n";
    cout << "- Складність: треба стежити за життєвим циклом об'єктів\n";
    cout << "- Неможливість створення незалежних копій без явного копіювання\n\n";
    
    cout << "ВИСНОВОК:\n";
    cout << "=========\n";
    cout << "• Посилання кращі для ефективності та швидкості\n";
    cout << "• Конструктор копій кращий для безпеки та незалежності\n";
    cout << "• Вибір залежить від конкретного застосування\n";
    cout << "• Часто використовують комбінацію обох підходів\n";
    cout << "• Для великих об'єктів краще використовувати посилання\n";
    cout << "• Для критичних даних краще використовувати копії\n";
    
    return 0;
}