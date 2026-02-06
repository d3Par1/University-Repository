#include <iostream>
#include <sstream>
#include <iomanip>

// Клас для перетворення рядка в double
class StringToDouble {
private:
    char buffer[100];  // Буфер для зберігання рядка
    double value;      // Значення після перетворення
    
public:
    StringToDouble() {
        buffer[0] = '\0';
        value = 0.0;
    }
    
    // Оператор введення для читання рядка
    friend std::istream& operator>>(std::istream& is, StringToDouble& obj) {
        is >> obj.buffer;
        // Перетворення буфера в double
        std::stringstream ss(obj.buffer);
        ss >> obj.value;
        if (ss.fail()) {
            obj.value = 0.0;  // Встановлюємо значення за замовчуванням при помилці
        }
        return is;
    }
    
    // Оператор виведення для відображення значення double
    friend std::ostream& operator<<(std::ostream& os, const StringToDouble& obj) {
        os << "Рядок: \"" << obj.buffer << "\", Значення double: " << obj.value;
        return os;
    }
    
    // Отримання значення double
    double getValue() const {
        return value;
    }
    
    // Отримання рядка
    const char* getString() const {
        return buffer;
    }
};

int main() {
    StringToDouble converter;
    
    std::cout << "Введіть число з плаваючою комою як рядок: ";
    std::cin >> converter;
    
    std::cout << "\nРезультат перетворення:\n" << converter << std::endl;
    
    // Використання перетвореного значення
    double result = converter.getValue() * 2;
    
    std::cout << "\nПодвоєне значення: " << result << std::endl;
    
    // Демонстрація перетворення декількох чисел
    StringToDouble values[3];
    
    std::cout << "\nВведіть три числа з плаваючою комою:\n";
    for (int i = 0; i < 3; i++) {
        std::cout << "Число " << (i + 1) << ": ";
        std::cin >> values[i];
    }
    
    std::cout << "\nВсі перетворені значення:" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << values[i] << std::endl;
    }
    
    double sum = 0.0;
    for (int i = 0; i < 3; i++) {
        sum += values[i].getValue();
    }
    
    std::cout << "\nСума всіх значень: " << sum << std::endl;
    
    return 0;
}