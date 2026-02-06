#include <iostream>
#include <cstdlib> // для atoi

class myclass {
private:
    int a;
    
public:
    // Лише перший конструктор позначений explicit
    explicit myclass(int x) { 
        a = x; 
        std::cout << "Конструктор myclass(int) викликаний з x = " << x << std::endl;
    }
    
    // Другий конструктор не explicit
    myclass(const char *str) { 
        a = atoi(str); 
        std::cout << "Конструктор myclass(const char*) викликаний з str = " << str << std::endl;
    }
    
    int getA() const { return a; }
    
    void display() const {
        std::cout << "Значення a: " << a << std::endl;
    }
};

void test(myclass obj) {
    std::cout << "Функція test() отримала об'єкт з a = " << obj.getA() << std::endl;
}

int main() {
    std::cout << "=== Тест 1: Конструктор з explicit ===\n";
    
    // Пряма ініціалізація - працює нормально
    myclass obj1(10);
    obj1.display();
    
    // Наступний рядок викликав би помилку, тому що myclass(int) є explicit
    // myclass obj2 = 20; // Помилка компіляції: не можна виконати неявне перетворення
    
    std::cout << "\n=== Тест 2: Конструктор без explicit ===\n";
    
    // Пряма ініціалізація
    myclass obj3("30");
    obj3.display();
    
    // Копіююча ініціалізація - працює, бо конструктор не explicit
    myclass obj4 = "40";
    obj4.display();
    
    std::cout << "\n=== Тест 3: Передача параметрів у функцію ===\n";
    
    // Наступний рядок викликав би помилку, тому що myclass(int) є explicit
    // test(50); // Помилка компіляції: не можна виконати неявне перетворення
    
    // Це працює, бо myclass(const char*) не є explicit
    test("60");
    
    std::cout << "\n=== Висновок ===\n";
    std::cout << "Якщо тільки myclass(int) позначений як explicit:\n";
    std::cout << "1. Неявне перетворення int в myclass заборонено\n";
    std::cout << "2. Неявне перетворення const char* в myclass дозволено\n";
    
    return 0;
}

// Якщо прибрати const з myclass(const char*):
/*
#include <iostream>
#include <cstdlib>

class myclass {
private:
    int a;
    
public:
    explicit myclass(int x) { a = x; }
    myclass(char *str) { a = atoi(str); } // Прибрали const
    
    void display() const {
        std::cout << "Значення a: " << a << std::endl;
    }
};

int main() {
    myclass obj1(10);
    
    // Наступні рядки НЕ скомпілюються:
    // myclass obj2 = "20"; // Помилка: не можна конвертувати const char* в char*
    // myclass obj3("30");  // Помилка: не можна конвертувати const char* в char*
    
    // Для використання конструктора без const потрібно:
    char str[] = "40";
    myclass obj4(str);
    
    return 0;
}
*/

// Коментар щодо видалення const:
// Якщо прибрати const з myclass(const char*), то компілятор видасть помилку
// при спробі використати рядковий літерал (який є const char*) для створення об'єкта.
// Це відбувається тому, що рядкові літерали в C++ мають тип const char*,
// і їх не можна передати в функцію, яка приймає char* без явного приведення типу,
// що в свою чергу не є безпечним і не рекомендується.