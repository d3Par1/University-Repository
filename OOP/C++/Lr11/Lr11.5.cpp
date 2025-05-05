#include <iostream>
#include <iomanip>

// Базовий клас Num
class Num {
protected:
    int value;
    
public:
    // Конструктор
    Num(int v = 0) : value(v) {}
    
    // Віртуальна функція для виведення числа
    virtual void shownum() {
        std::cout << "Десяткове: " << value << std::endl;
    }
    
    // Функція для зміни значення
    void setValue(int v) {
        value = v;
    }
    
    // Віртуальний деструктор
    virtual ~Num() {}
};

// Похідний клас для виведення в шістнадцятковій системі
class outhex : public Num {
public:
    // Конструктор
    outhex(int v = 0) : Num(v) {}
    
    // Перевизначена функція для виведення числа в шістнадцятковій системі
    void shownum() override {
        std::cout << "Шістнадцяткове: 0x" << std::hex << std::uppercase << value << std::dec << std::endl;
    }
};

// Похідний клас для виведення у вісімковій системі
class outoct : public Num {
public:
    // Конструктор
    outoct(int v = 0) : Num(v) {}
    
    // Перевизначена функція для виведення числа у вісімковій системі
    void shownum() override {
        std::cout << "Вісімкове: 0" << std::oct << value << std::dec << std::endl;
    }
};

int main() {
    // Створюємо об'єкти різних класів
    Num decimal(255);
    outhex hex(255);
    outoct oct(255);
    
    // Виводимо значення
    std::cout << "Демонстрація виведення числа 255 у різних системах числення:\n" << std::endl;
    
    std::cout << "Використання базового класу Num:" << std::endl;
    decimal.shownum();
    std::cout << std::endl;
    
    std::cout << "Використання класу outhex:" << std::endl;
    hex.shownum();
    std::cout << std::endl;
    
    std::cout << "Використання класу outoct:" << std::endl;
    oct.shownum();
    std::cout << std::endl;
    
    // Демонстрація поліморфізму
    std::cout << "Демонстрація поліморфізму з використанням вказівників на базовий клас:" << std::endl;
    
    Num* ptr;
    
    ptr = &decimal;
    ptr->shownum();
    
    ptr = &hex;
    ptr->shownum();
    
    ptr = &oct;
    ptr->shownum();
    
    return 0;
}