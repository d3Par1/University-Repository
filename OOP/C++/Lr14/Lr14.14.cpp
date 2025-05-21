#include <iostream>
#include <string>

class Example {
private:
    int data;
    std::string name;
    static int instanceCount;
    
public:
    // Конструктор
    Example(int d, const std::string& n) : data(d), name(n) {
        instanceCount++;
    }
    
    // Деструктор
    ~Example() {
        instanceCount--;
    }
    
    // Функція, яка має бути const, оскільки не змінює стан об'єкта
    void display() const {
        std::cout << "Ім'я: " << name << ", Дані: " << data << std::endl;
    }
    
    // Функція, яка має бути const, оскільки лише повертає значення, не змінюючи об'єкт
    int getData() const {
        return data;
    }
    
    // Функція, яка не може бути const, оскільки змінює стан об'єкта
    void setData(int d) {
        data = d;
    }
    
    // Функція, яка має бути static, оскільки працює з даними всього класу, а не екземпляра
    static int getInstanceCount() {
        return instanceCount;
    }
    
    // Допоміжна функція, яка не залежить від змінних екземпляра
    // Має бути static, оскільки не потребує доступу до членів екземпляра
    static bool isPositive(int value) {
        return value > 0;
    }
};

// Ініціалізація статичного члена
int Example::instanceCount = 0;

// Приклад з простором імен
namespace utilities {
    // Функція в просторі імен, яка могла б бути const, якби була методом класу
    void printValue(const int& value) {
        std::cout << "Значення: " << value << std::endl;
    }
    
    // Функція, яка могла б бути static в класі
    double calculateSquare(double x) {
        return x * x;
    }
}

int main() {
    Example e1(10, "Перший");
    Example e2(20, "Другий");
    
    // Використання const методів
    e1.display();
    std::cout << "Дані з e1: " << e1.getData() << std::endl;
    
    // Використання static методу
    std::cout << "Загальна кількість екземплярів: " << Example::getInstanceCount() << std::endl;
    
    // Використання static допоміжної функції
    int testValue = 15;
    std::cout << "Чи " << testValue << " додатній? " 
              << (Example::isPositive(testValue) ? "Так" : "Ні") << std::endl;
    
    // Використання функції з простору імен
    utilities::printValue(42);
    
    std::cout << "\nАналіз функцій, які могли б бути const або static:" << std::endl;
    std::cout << "1. Методи, які тільки читають дані з об'єкта, повинні бути const:" << std::endl;
    std::cout << "   - getData(), display()" << std::endl;
    
    std::cout << "2. Методи, які модифікують дані об'єкта, не можуть бути const:" << std::endl;
    std::cout << "   - setData()" << std::endl;
    
    std::cout << "3. Методи, які не використовують змінні екземпляра, повинні бути static:" << std::endl;
    std::cout << "   - isPositive(), getInstanceCount()" << std::endl;
    
    std::cout << "4. Функції в просторі імен вже віддалені від конкретних екземплярів:" << std::endl;
    std::cout << "   - utilities::printValue(), utilities::calculateSquare()" << std::endl;
    
    return 0;
}