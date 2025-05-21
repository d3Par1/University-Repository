#include <iostream>
#include <cstring>

class strtype {
    char *str;      // вказівник на рядок
    int len;        // довжина рядка
public:
    // Конструктор
    strtype(const char *s) {
        len = strlen(s);
        str = new char[len+1];
        strcpy(str, s);
    }
    
    // Деструктор
    ~strtype() {
        delete[] str;
    }
    
    // Функція перетворення в ціле
    operator int() const {
        return len;  // Повертає довжину рядка
    }
    
    // Відображення рядка
    void show() const {
        std::cout << str << std::endl;
    }
};

int main() {
    strtype s1("Це тестовий рядок");
    
    std::cout << "Рядок: ";
    s1.show();
    
    // Використовуємо функцію перетворення
    int length = s1;  // Неявне перетворення типу strtype в int
    
    std::cout << "Довжина рядка: " << length << std::endl;
    
    // Явне перетворення
    std::cout << "Ще раз довжина: " << static_cast<int>(s1) << std::endl;
    
    return 0;
}