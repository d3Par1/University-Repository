#include <iostream>

class Counter {
private:
    static int objectCount;  // Статична змінна для підрахунку об'єктів
    int id;                  // Ідентифікатор об'єкту
    
public:
    // Конструктор
    Counter() {
        objectCount++;       // Збільшуємо лічильник при створенні об'єкту
        id = objectCount;
        std::cout << "Конструктор викликаний. Об'єкт #" << id << " створено." << std::endl;
        std::cout << "Загальна кількість об'єктів: " << objectCount << std::endl;
    }
    
    // Деструктор
    ~Counter() {
        std::cout << "Деструктор викликаний. Об'єкт #" << id << " знищено." << std::endl;
        objectCount--;       // Зменшуємо лічильник при знищенні об'єкту
        std::cout << "Загальна кількість об'єктів: " << objectCount << std::endl;
    }
    
    // Статичний метод для отримання кількості об'єктів
    static int getCount() {
        return objectCount;
    }
};

// Ініціалізація статичної змінної
int Counter::objectCount = 0;

int main() {
    std::cout << "Початкова кількість об'єктів: " << Counter::getCount() << std::endl;
    
    // Створюємо блоки видимості для контролю часу життя об'єктів
    {
        std::cout << "\nСтворюємо перший об'єкт:" << std::endl;
        Counter c1;
        
        {
            std::cout << "\nСтворюємо другий об'єкт:" << std::endl;
            Counter c2;
            
            std::cout << "\nСтворюємо третій об'єкт:" << std::endl;
            Counter c3;
            
            std::cout << "\nВиходимо з внутрішнього блоку..." << std::endl;
        } // c2 і c3 знищуються тут
        
        std::cout << "\nПовернулись у зовнішній блок." << std::endl;
        std::cout << "Поточна кількість об'єктів: " << Counter::getCount() << std::endl;
    } // c1 знищується тут
    
    std::cout << "\nФінальна кількість об'єктів: " << Counter::getCount() << std::endl;
    
    return 0;
}