#include <iostream>
#include <cstring>
using namespace std;

class strtype {
    static const int MAX_SIZE = 100;  // Фіксована довжина рядків
    char str[MAX_SIZE];
    
public:
    // Конструктори
    strtype() {
        str[0] = '\0';
        cout << "Створено порожній strtype\n";
    }
    
    strtype(const char *s) {
        if(strlen(s) < MAX_SIZE) {
            strcpy(str, s);
        } else {
            strncpy(str, s, MAX_SIZE - 1);
            str[MAX_SIZE - 1] = '\0';
        }
        cout << "Створено strtype: \"" << str << "\"\n";
    }
    
    strtype(const strtype &other) {
        strcpy(str, other.str);
        cout << "Створено копію strtype: \"" << str << "\"\n";
    }
    
    // Деструктор
    ~strtype() {
        cout << "Деструктор strtype: \"" << str << "\"\n";
    }
    
    // Оператор присвоєння (=)
    strtype& operator=(const strtype &other) {
        if(this != &other) {
            strcpy(str, other.str);
            cout << "Присвоєння: \"" << str << "\"\n";
        }
        return *this;
    }
    
    strtype& operator=(const char *s) {
        if(strlen(s) < MAX_SIZE) {
            strcpy(str, s);
        } else {
            strncpy(str, s, MAX_SIZE - 1);
            str[MAX_SIZE - 1] = '\0';
        }
        cout << "Присвоєння рядка: \"" << str << "\"\n";
        return *this;
    }
    
    // Оператор конкатенації (+)
    strtype operator+(const strtype &other) const {
        strtype result;
        
        if(strlen(str) + strlen(other.str) < MAX_SIZE) {
            strcpy(result.str, str);
            strcat(result.str, other.str);
        } else {
            // Якщо результат не поміститься, обрізаємо
            strcpy(result.str, str);
            int remaining = MAX_SIZE - strlen(str) - 1;
            if(remaining > 0) {
                strncat(result.str, other.str, remaining);
            }
        }
        
        cout << "Конкатенація: \"" << str << "\" + \"" << other.str 
             << "\" = \"" << result.str << "\"\n";
        return result;
    }
    
    strtype operator+(const char *s) const {
        strtype result;
        
        if(strlen(str) + strlen(s) < MAX_SIZE) {
            strcpy(result.str, str);
            strcat(result.str, s);
        } else {
            strcpy(result.str, str);
            int remaining = MAX_SIZE - strlen(str) - 1;
            if(remaining > 0) {
                strncat(result.str, s, remaining);
            }
        }
        
        cout << "Конкатенація: \"" << str << "\" + \"" << s 
             << "\" = \"" << result.str << "\"\n";
        return result;
    }
    
    // Оператори порівняння
    bool operator<(const strtype &other) const {
        return strcmp(str, other.str) < 0;
    }
    
    bool operator>(const strtype &other) const {
        return strcmp(str, other.str) > 0;
    }
    
    bool operator==(const strtype &other) const {
        return strcmp(str, other.str) == 0;
    }
    
    bool operator!=(const strtype &other) const {
        return strcmp(str, other.str) != 0;
    }
    
    bool operator<=(const strtype &other) const {
        return strcmp(str, other.str) <= 0;
    }
    
    bool operator>=(const strtype &other) const {
        return strcmp(str, other.str) >= 0;
    }
    
    // Порівняння з C-рядками
    bool operator<(const char *s) const {
        return strcmp(str, s) < 0;
    }
    
    bool operator>(const char *s) const {
        return strcmp(str, s) > 0;
    }
    
    bool operator==(const char *s) const {
        return strcmp(str, s) == 0;
    }
    
    // Оператор +=
    strtype& operator+=(const strtype &other) {
        *this = *this + other;
        return *this;
    }
    
    strtype& operator+=(const char *s) {
        *this = *this + s;
        return *this;
    }
    
    // Методи доступу
    const char* c_str() const { return str; }
    int length() const { return strlen(str); }
    int max_size() const { return MAX_SIZE; }
    bool empty() const { return str[0] == '\0'; }
    
    // Оператор індексування
    char& operator[](int index) {
        if(index >= 0 && index < strlen(str)) {
            return str[index];
        }
        cout << "Помилка індексу " << index << "\n";
        return str[0];  // Повертаємо перший символ у випадку помилки
    }
    
    const char& operator[](int index) const {
        if(index >= 0 && index < strlen(str)) {
            return str[index];
        }
        cout << "Помилка індексу " << index << "\n";
        return str[0];
    }
    
    // Допоміжні методи
    void show() const {
        cout << "\"" << str << "\"";
    }
    
    void to_upper() {
        for(int i = 0; str[i]; i++) {
            if(str[i] >= 'a' && str[i] <= 'z') {
                str[i] = str[i] - 'a' + 'A';
            }
        }
    }
    
    void to_lower() {
        for(int i = 0; str[i]; i++) {
            if(str[i] >= 'A' && str[i] <= 'Z') {
                str[i] = str[i] - 'A' + 'a';
            }
        }
    }
    
    int find(const char *substr) const {
        char *pos = strstr(str, substr);
        if(pos) {
            return pos - str;
        }
        return -1;  // Не знайдено
    }
    
    strtype substring(int start, int len) const {
        strtype result;
        if(start >= 0 && start < strlen(str)) {
            int actual_len = (len < strlen(str) - start) ? len : strlen(str) - start;
            strncpy(result.str, str + start, actual_len);
            result.str[actual_len] = '\0';
        }
        return result;
    }
};

// Дружня функція для операції char* + strtype
strtype operator+(const char *s, const strtype &obj) {
    strtype result(s);
    return result + obj;
}

int main() {
    cout << "ДЕМОНСТРАЦІЯ ПОВНОГО КЛАСУ strtype:\n";
    cout << "===================================\n\n";
    
    // Створення рядків
    cout << "1. СТВОРЕННЯ РЯДКІВ:\n";
    cout << "===================\n";
    
    strtype str1("Hello");
    strtype str2("World");
    strtype str3;  // Порожній рядок
    strtype str4(str1);  // Копіювання
    
    cout << "\nСтворені рядки:\n";
    cout << "str1: "; str1.show(); cout << " (довжина: " << str1.length() << ")\n";
    cout << "str2: "; str2.show(); cout << " (довжина: " << str2.length() << ")\n";
    cout << "str3: "; str3.show(); cout << " (порожній: " << (str3.empty() ? "так" : "ні") << ")\n";
    cout << "str4: "; str4.show(); cout << " (копія str1)\n";
    
    // Оператор присвоєння
    cout << "\n2. ОПЕРАТОР ПРИСВОЄННЯ (=):\n";
    cout << "===========================\n";
    
    cout << "str3 до присвоєння: "; str3.show(); cout << "\n";
    str3 = str2;
    cout << "str3 після str3 = str2: "; str3.show(); cout << "\n";
    
    str3 = "Programming";
    cout << "str3 після str3 = \"Programming\": "; str3.show(); cout << "\n";
    
    // Ланцюгове присвоєння
    strtype str5, str6;
    str5 = str6 = str1;
    cout << "Ланцюгове присвоєння str5 = str6 = str1:\n";
    cout << "str5: "; str5.show(); cout << "\n";
    cout << "str6: "; str6.show(); cout << "\n";
    
    // Конкатенація рядків
    cout << "\n3. КОНКАТЕНАЦІЯ РЯДКІВ (+):\n";
    cout << "===========================\n";
    
    strtype result = str1 + str2;
    cout << "result: "; result.show(); cout << "\n";
    
    result = str1 + " " + str2;
    cout << "str1 + \" \" + str2: "; result.show(); cout << "\n";
    
    result = str1 + ", ";
    cout << "str1 + \", \": "; result.show(); cout << "\n";
    
    result = "Hi, " + str2;
    cout << "\"Hi, \" + str2: "; result.show(); cout << "\n";
    
    // Оператор +=
    cout << "\n4. ОПЕРАТОР += (ДОДАВАННЯ З ПРИСВОЄННЯМ):\n";
    cout << "========================================\n";
    
    strtype builder("Start");
    cout << "builder: "; builder.show(); cout << "\n";
    
    builder += " Middle";
    cout << "builder += \" Middle\": "; builder.show(); cout << "\n";
    
    builder += strtype(" End");
    cout << "builder += strtype(\" End\"): "; builder.show(); cout << "\n";
    
    // Порівняння рядків
    cout << "\n5. ПОРІВНЯННЯ РЯДКІВ:\n";
    cout << "====================\n";
    
    strtype alpha("Alpha");
    strtype beta("Beta");
    strtype alpha2("Alpha");
    
    cout << "alpha: "; alpha.show(); cout << "\n";
    cout << "beta: "; beta.show(); cout << "\n";
    cout << "alpha2: "; alpha2.show(); cout << "\n";
    
    cout << "\nРезультати порівняння:\n";
    cout << "alpha == alpha2: " << (alpha == alpha2 ? "true" : "false") << "\n";
    cout << "alpha == beta: " << (alpha == beta ? "true" : "false") << "\n";
    cout << "alpha != beta: " << (alpha != beta ? "true" : "false") << "\n";
    cout << "alpha < beta: " << (alpha < beta ? "true" : "false") << " (лексикографічно)\n";
    cout << "alpha > beta: " << (alpha > beta ? "true" : "false") << "\n";
    cout << "alpha <= alpha2: " << (alpha <= alpha2 ? "true" : "false") << "\n";
    cout << "alpha >= alpha2: " << (alpha >= alpha2 ? "true" : "false") << "\n";
    
    // Порівняння з C-рядками
    cout << "\nПорівняння з C-рядками:\n";
    cout << "alpha == \"Alpha\": " << (alpha == "Alpha" ? "true" : "false") << "\n";
    cout << "alpha < \"Zebra\": " << (alpha < "Zebra" ? "true" : "false") << "\n";
    cout << "beta > \"Apple\": " << (beta > "Apple" ? "true" : "false") << "\n";
    
    // Індексування
    cout << "\n6. ІНДЕКСУВАННЯ ([]):\n";
    cout << "====================\n";
    
    strtype word("Programming");
    cout << "word: "; word.show(); cout << "\n";
    
    cout << "Символи word:\n";
    for(int i = 0; i < word.length(); i++) {
        cout << "word[" << i << "] = '" << word[i] << "'\n";
    }
    
    cout << "\nЗміна символів:\n";
    cout << "word[0] до зміни: '" << word[0] << "'\n";
    word[0] = 'p';  // P -> p
    cout << "word[0] після word[0] = 'p': '" << word[0] << "'\n";
    cout << "word тепер: "; word.show(); cout << "\n";
    
    // Робота з регістром
    cout << "\n7. РОБОТА З РЕГІСТРОМ:\n";
    cout << "=====================\n";
    
    strtype text("Mixed CaSe Text");
    cout << "Оригінальний text: "; text.show(); cout << "\n";
    
    strtype upper_text = text;
    upper_text.to_upper();
    cout << "Верхній регістр: "; upper_text.show(); cout << "\n";
    
    strtype lower_text = text;
    lower_text.to_lower();
    cout << "Нижній регістр: "; lower_text.show(); cout << "\n";
    
    // Пошук підрядків
    cout << "\n8. ПОШУК ПІДРЯДКІВ:\n";
    cout << "==================\n";
    
    strtype sentence("The quick brown fox jumps");
    cout << "sentence: "; sentence.show(); cout << "\n";
    
    int pos = sentence.find("quick");
    cout << "Позиція \"quick\": " << pos << "\n";
    
    pos = sentence.find("fox");
    cout << "Позиція \"fox\": " << pos << "\n";
    
    pos = sentence.find("cat");
    cout << "Позиція \"cat\": " << pos << " (не знайдено)\n";
    
    // Підрядки
    cout << "\n9. ВИДІЛЕННЯ ПІДРЯДКІВ:\n";
    cout << "======================\n";
    
    strtype sample("Hello World Programming");
    cout << "sample: "; sample.show(); cout << "\n";
    
    strtype sub1 = sample.substring(0, 5);
    cout << "substring(0, 5): "; sub1.show(); cout << "\n";
    
    strtype sub2 = sample.substring(6, 5);
    cout << "substring(6, 5): "; sub2.show(); cout << "\n";
    
    strtype sub3 = sample.substring(12, 20);
    cout << "substring(12, 20): "; sub3.show(); cout << "\n";
    
    // Практичні приклади
    cout << "\n10. ПРАКТИЧНІ ПРИКЛАДИ:\n";
    cout << "======================\n";
    
    // Побудова речення
    strtype greeting = "Hello";
    strtype name = "John";
    strtype punctuation = "!";
    
    strtype message = greeting + ", " + name + punctuation;
    cout << "Побудова привітання: "; message.show(); cout << "\n";
    
    // Список слів
    strtype words[] = {"Apple", "Banana", "Cherry", "Date"};
    strtype word_list;
    
    for(int i = 0; i < 4; i++) {
        if(i > 0) word_list += ", ";
        word_list += words[i];
    }
    cout << "Список слів: "; word_list.show(); cout << "\n";
    
    // Сортування рядків
    cout << "\n11. СОРТУВАННЯ РЯДКІВ:\n";
    cout << "=====================\n";
    
    strtype strings[] = {"Zebra", "Apple", "Mango", "Banana"};
    int n = 4;
    
    cout << "До сортування:\n";
    for(int i = 0; i < n; i++) {
        cout << i << ": "; strings[i].show(); cout << "\n";
    }
    
    // Bubble sort
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < n-i-1; j++) {
            if(strings[j] > strings[j+1]) {
                strtype temp = strings[j];
                strings[j] = strings[j+1];
                strings[j+1] = temp;
            }
        }
    }
    
    cout << "\nПісля сортування:\n";
    for(int i = 0; i < n; i++) {
        cout << i << ": "; strings[i].show(); cout << "\n";
    }
    
    // Обмеження довжини
    cout << "\n12. ОБМЕЖЕННЯ ДОВЖИНИ:\n";
    cout << "=====================\n";
    
    cout << "Максимальна довжина рядка: " << strtype().max_size() << "\n";
    
    // Спроба створити довгий рядок
    strtype long_string;
    for(int i = 0; i < 15; i++) {
        long_string += "Very Long String ";
    }
    cout << "Довгий рядок (обрізано): "; long_string.show(); cout << "\n";
    cout << "Довжина: " << long_string.length() << "\n";
    
    // Ланцюгові операції
    cout << "\n13. ЛАНЦЮГОВІ ОПЕРАЦІЇ:\n";
    cout << "======================\n";
    
    strtype chain_result = strtype("Start") + " -> " + strtype("Middle") + " -> " + "End";
    cout << "Ланцюгова конкатенація: "; chain_result.show(); cout << "\n";
    
    // Порівняння в умовах
    if(str1 < str2) {
        cout << "str1 "; str1.show(); cout << " менше за str2 "; str2.show(); cout << "\n";
    } else {
        cout << "str1 "; str1.show(); cout << " не менше за str2 "; str2.show(); cout << "\n";
    }
    
    cout << "\n14. ПЕРЕВАГИ РЕАЛІЗАЦІЇ:\n";
    cout << "========================\n";
    cout << "+ Фіксована довжина - передбачувана продуктивність\n";
    cout << "+ Повний набір операторів для зручності використання\n";
    cout << "+ Автоматичне управління пам'яттю\n";
    cout << "+ Сумісність з C-рядками\n";
    cout << "+ Безпечні операції копіювання та присвоєння\n";
    cout << "+ Інтуїтивний синтаксис\n";
    
    cout << "\nОБМЕЖЕННЯ:\n";
    cout << "- Фіксована максимальна довжина\n";
    cout << "- Обрізання довгих рядків\n";
    cout << "- Витрати пам'яті навіть для коротких рядків\n";
    
    return 0;
}