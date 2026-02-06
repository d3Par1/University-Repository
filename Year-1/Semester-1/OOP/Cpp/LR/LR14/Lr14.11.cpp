#include <iostream>
#include <cstring>

// Клас для роботи з рядками
class StringWrapper {
private:
    char* str;
    
public:
    // Конструктор, що дозволяє автоматичне перетворення з const char*
    StringWrapper(const char* s = "") {
        str = new char[strlen(s) + 1];
        strcpy(str, s);
        std::cout << "Конструктор викликаний для: \"" << str << "\"" << std::endl;
    }
    
    // Деструктор
    ~StringWrapper() {
        std::cout << "Деструктор викликаний для: \"" << str << "\"" << std::endl;
        delete[] str;
    }
    
    // Копіюючий конструктор (потрібен для правильного глибокого копіювання)
    StringWrapper(const StringWrapper& other) {
        str = new char[strlen(other.str) + 1];
        strcpy(str, other.str);
        std::cout << "Копіюючий конструктор викликаний для: \"" << str << "\"" << std::endl;
    }
    
    // Перевантажений оператор присвоювання (глибоке копіювання)
    StringWrapper& operator=(const StringWrapper& other) {
        std::cout << "Оператор присвоювання викликаний" << std::endl;
        if (this != &other) {
            delete[] str;
            str = new char[strlen(other.str) + 1];
            strcpy(str, other.str);
            std::cout << "  Нове значення: \"" << str << "\"" << std::endl;
        }
        return *this;
    }
    
    // Відображення рядка
    void display() const {
        std::cout << "Рядок: \"" << str << "\"" << std::endl;
    }
};

int main() {
    StringWrapper s1("Привіт");
    s1.display();
    
    // Пряме присвоєння з const char* (використовує конструктор для перетворення)
    s1 = "Світ"; 
    s1.display();
    
    // Ось що відбувається:
    // 1. "Світ" перетворюється в тимчасовий об'єкт StringWrapper через конструктор
    // 2. Викликається оператор присвоювання для StringWrapper з цим тимчасовим об'єктом
    // 3. Тимчасовий об'єкт знищується після присвоювання
    
    std::cout << "\nПояснення:" << std::endl;
    std::cout << "Навіть при автоматичному перетворенні з const char* до StringWrapper," << std::endl;
    std::cout << "нам все одно потрібен перевантажений оператор присвоювання для правильного" << std::endl;
    std::cout << "глибокого копіювання. Автоматичне перетворення створює тимчасовий об'єкт," << std::endl;
    std::cout << "але присвоювання між об'єктами все одно вимагає належної обробки." << std::endl;
    
    std::cout << "\nСпроба без оператора присвоювання призведе до:" << std::endl;
    std::cout << "1. Втрати пам'яті (memory leak) - оскільки стара пам'ять не буде звільнена" << std::endl;
    std::cout << "2. Неправильного керування ресурсами - вказівники на одні й ті ж адреси" << std::endl;
    std::cout << "3. Помилок при видаленні пам'яті при знищенні об'єктів" << std::endl;
    
    return 0;
}