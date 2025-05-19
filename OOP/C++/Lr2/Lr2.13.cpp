#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

// Перевантажена функція rev_str() з одним параметром
// Змінює рядок на місці
void rev_str(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// Перевантажена функція rev_str() з двома параметрами
// Зберігає результат у другому параметрі
void rev_str(const char* src, char* dest) {
    int len = strlen(src);
    
    // Копіюємо символи у зворотньому порядку
    for (int i = 0; i < len; i++) {
        dest[i] = src[len - i - 1];
    }
    
    // Додаємо нульовий символ в кінці
    dest[len] = '\0';
}

int main() {
    char s1[80], s2[80];
    
    // Тестуємо функцію з двома параметрами
    strcpy(s1, "hello");
    cout << "Оригінальний рядок s1: " << s1 << endl;
    
    rev_str(s1, s2);
    cout << "Після rev_str(s1, s2):" << endl;
    cout << "s1: " << s1 << endl;
    cout << "s2: " << s2 << endl;
    
    // Тестуємо функцію з одним параметром
    cout << "\nТепер використаємо rev_str(s1) для зміни s1:" << endl;
    rev_str(s1);
    cout << "s1: " << s1 << endl;
    
    // Ще один тест з довшим рядком
    strcpy(s1, "abcdefghij");
    cout << "\nНовий рядок s1: " << s1 << endl;
    
    rev_str(s1, s2);
    cout << "Після rev_str(s1, s2):" << endl;
    cout << "s1: " << s1 << endl;
    cout << "s2: " << s2 << endl;
    
    rev_str(s1);
    cout << "Після rev_str(s1):" << endl;
    cout << "s1: " << s1 << endl;
    
    return 0;
}