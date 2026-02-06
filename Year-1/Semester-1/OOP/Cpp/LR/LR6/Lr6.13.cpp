#include <iostream>
#include <cstring>
using namespace std;

class strtype {
    char *p;
    int len;
    
public:
    strtype(const char *ptr);
    strtype(const strtype &obj);  // Конструктор копій
    strtype();  // Конструктор за замовчуванням
    ~strtype() { 
        delete[] p; 
        cout << "Деструктор викликано для рядка\n";
    }
    
    char *get() { return p; }
    const char *get() const { return p; }
    int length() const { return len; }
    
    // Перевантаження оператора [] для читання та запису
    char& operator[](int i);
    const char& operator[](int i) const;  // Константна версія
    
    strtype& operator=(const strtype &obj);
    
    void show() const {
        cout << "\"" << (p ? p : "null") << "\" (довжина: " << len << ")";
    }
    
    // Допоміжні методи
    bool is_valid_index(int i) const {
        return i >= 0 && i < len;
    }
    
    void to_upper();
    void to_lower();
    char* find_char(char c);
};

strtype::strtype(const char *ptr) {
    len = strlen(ptr);
    p = new char[len + 1];
    strcpy(p, ptr);
    cout << "Створено strtype: ";
    show();
    cout << "\n";
}

strtype::strtype() {
    len = 0;
    p = new char[1];
    p[0] = '\0';
    cout << "Створено порожній strtype\n";
}

strtype::strtype(const strtype &obj) {
    len = obj.len;
    p = new char[len + 1];
    strcpy(p, obj.p);
    cout << "Створено копію strtype: ";
    show();
    cout << "\n";
}

// Перевантаження оператора [] (non-const версія)
char& strtype::operator[](int i) {
    if(i < 0 || i >= len) {
        cout << "Помилка: індекс " << i << " виходить за межі рядка [0.." << len-1 << "]\n";
        cout << "Повертаємо посилання на перший символ\n";
        return p[0];  // Повертаємо перший символ у випадку помилки
    }
    return p[i];
}

// Перевантаження оператора [] (const версія)
const char& strtype::operator[](int i) const {
    if(i < 0 || i >= len) {
        cout << "Помилка: індекс " << i << " виходить за межі рядка [0.." << len-1 << "]\n";
        cout << "Повертаємо посилання на перший символ\n";
        return p[0];
    }
    return p[i];
}

strtype& strtype::operator=(const strtype &obj) {
    if(this == &obj) return *this;
    
    delete[] p;
    len = obj.len;
    p = new char[len + 1];
    strcpy(p, obj.p);
    cout << "Присвоєння strtype: ";
    show();
    cout << "\n";
    return *this;
}

void strtype::to_upper() {
    for(int i = 0; i < len; i++) {
        if(p[i] >= 'a' && p[i] <= 'z') {
            p[i] = p[i] - 'a' + 'A';
        }
    }
}

void strtype::to_lower() {
    for(int i = 0; i < len; i++) {
        if(p[i] >= 'A' && p[i] <= 'Z') {
            p[i] = p[i] - 'A' + 'a';
        }
    }
}

char* strtype::find_char(char c) {
    for(int i = 0; i < len; i++) {
        if(p[i] == c) {
            return &p[i];
        }
    }
    return nullptr;
}

int main() {
    cout << "ДЕМОНСТРАЦІЯ ОПЕРАТОРА [] ДЛЯ КЛАСУ strtype:\n";
    cout << "============================================\n\n";
    
    // Створення рядків
    cout << "1. СТВОРЕННЯ РЯДКІВ:\n";
    cout << "===================\n";
    
    strtype str1("Hello World");
    strtype str2("Programming");
    strtype str3;
    
    cout << "\nСтворені рядки:\n";
    cout << "str1: "; str1.show(); cout << "\n";
    cout << "str2: "; str2.show(); cout << "\n";
    cout << "str3: "; str3.show(); cout << "\n";
    
    // Читання символів через оператор []
    cout << "\n2. ЧИТАННЯ СИМВОЛІВ:\n";
    cout << "===================\n";
    
    cout << "Символи str1 \"" << str1.get() << "\":\n";
    for(int i = 0; i < str1.length(); i++) {
        cout << "str1[" << i << "] = '" << str1[i] << "'\n";
    }
    
    cout << "\nПерші та останні символи:\n";
    cout << "str1[0] = '" << str1[0] << "' (перший символ)\n";
    cout << "str1[" << str1.length()-1 << "] = '" << str1[str1.length()-1] << "' (останній символ)\n";
    
    // Запис символів через оператор []
    cout << "\n3. ЗАПИС СИМВОЛІВ (ЛІВА ЧАСТИНА ПРИСВОЄННЯ):\n";
    cout << "==========================================\n";
    
    cout << "str1 до зміни: "; str1.show(); cout << "\n";
    
    // Заміна символів
    str1[0] = 'h';  // H -> h
    str1[6] = 'w';  // W -> w
    
    cout << "Після зміни str1[0]='h' та str1[6]='w': "; str1.show(); cout << "\n";
    
    // Заміна слова
    cout << "\nЗаміна 'World' на 'Earth':\n";
    str1[6] = 'E';
    str1[7] = 'a';
    str1[8] = 'r';
    str1[9] = 't';
    str1[10] = 'h';
    
    cout << "Результат: "; str1.show(); cout << "\n";
    
    // Демонстрація контролю кордонів
    cout << "\n4. КОНТРОЛЬ КОРДОНІВ:\n";
    cout << "====================\n";
    
    cout << "Спроба доступу до коректних індексів:\n";
    cout << "str2[0] = '" << str2[0] << "'\n";
    cout << "str2[" << str2.length()-1 << "] = '" << str2[str2.length()-1] << "'\n";
    
    cout << "\nСпроба доступу до некоректних індексів:\n";
    char bad_char = str2[100];  // Помилка
    cout << "str2[100] повернув: '" << bad_char << "'\n";
    
    // Константний об'єкт
    cout << "\n5. РОБОТА З КОНСТАНТНИМ ОБ'ЄКТОМ:\n";
    cout << "===============================\n";
    
    const strtype const_str("Constant String");
    cout << "Константний рядок: "; const_str.show(); cout << "\n";
    
    cout << "Читання з константного рядка:\n";
    for(int i = 0; i < 8; i++) {  // Перші 8 символів
        cout << "const_str[" << i << "] = '" << const_str[i] << "'\n";
    }
    
    // Наступний рядок НЕ скомпілюється для константного об'єкта:
    // const_str[0] = 'c';  // ПОМИЛКА! Не можна змінювати константний об'єкт
    
    // Практичні приклади використання
    cout << "\n6. ПРАКТИЧНІ ПРИКЛАДИ:\n";
    cout << "=====================\n";
    
    strtype message("Hello Programming World");
    cout << "Оригінальний рядок: "; message.show(); cout << "\n";
    
    // Підрахунок голосних літер
    cout << "\nПідрахунок голосних літер:\n";
    int vowel_count = 0;
    const char vowels[] = "aeiouAEIOU";
    
    for(int i = 0; i < message.length(); i++) {
        for(int j = 0; vowels[j] != '\0'; j++) {
            if(message[i] == vowels[j]) {
                vowel_count++;
                cout << "Знайдено голосну '" << message[i] << "' на позиції " << i << "\n";
                break;
            }
        }
    }
    cout << "Загальна кількість голосних: " << vowel_count << "\n";
    
    // Зміна регістру через оператор []
    cout << "\n7. ЗМІНА РЕГІСТРУ:\n";
    cout << "=================\n";
    
    strtype text("MiXeD CaSe TeXt");
    cout << "Оригінальний текст: "; text.show(); cout << "\n";
    
    // Переведення в нижній регістр через оператор []
    cout << "Переведення в нижній регістр через []:\n";
    for(int i = 0; i < text.length(); i++) {
        if(text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = text[i] - 'A' + 'a';
        }
    }
    cout << "Результат: "; text.show(); cout << "\n";
    
    // Пошук та заміна символів
    cout << "\n8. ПОШУК ТА ЗАМІНА:\n";
    cout << "==================\n";
    
    strtype sentence("The quick brown fox");
    cout << "Оригінальне речення: "; sentence.show(); cout << "\n";
    
    // Заміна всіх пробілів на підкреслення
    cout << "Заміна пробілів на підкреслення:\n";
    for(int i = 0; i < sentence.length(); i++) {
        if(sentence[i] == ' ') {
            sentence[i] = '_';
            cout << "Замінено пробіл на позиції " << i << "\n";
        }
    }
    cout << "Результат: "; sentence.show(); cout << "\n";
    
    // Реверс рядка
    cout << "\n9. РЕВЕРС РЯДКА:\n";
    cout << "===============\n";
    
    strtype reverse_str("ABCDEF");
    cout << "До реверсу: "; reverse_str.show(); cout << "\n";
    
    // Алгоритм реверсу
    int length = reverse_str.length();
    for(int i = 0; i < length / 2; i++) {
        char temp = reverse_str[i];
        reverse_str[i] = reverse_str[length - 1 - i];
        reverse_str[length - 1 - i] = temp;
    }
    
    cout << "Після реверсу: "; reverse_str.show(); cout << "\n";
    
    // Порівняння з традиційними методами
    cout << "\n10. ПОРІВНЯННЯ З ТРАДИЦІЙНИМИ МЕТОДАМИ:\n";
    cout << "======================================\n";
    
    strtype demo_str("Demo String");
    
    cout << "Доступ через оператор []: " << demo_str[0] << demo_str[5] << "\n";
    cout << "Доступ через get(): " << demo_str.get()[0] << demo_str.get()[5] << "\n";
    
    cout << "\nПЕРЕВАГИ ОПЕРАТОРА []:\n";
    cout << "+ Природний синтаксис\n";
    cout << "+ Можна використовувати в лівій частині присвоєння\n";
    cout << "+ Автоматичний контроль кордонів\n";
    cout << "+ Підтримка константних об'єктів\n";
    cout << "+ Інтуїтивно зрозумілий код\n";
    
    cout << "\nОБМЕЖЕННЯ:\n";
    cout << "- Додаткові перевірки сповільнюють роботу\n";
    cout << "- Потребує обережності з індексами\n";
    cout << "- Не змінює довжину рядка\n";
    
    return 0;
}