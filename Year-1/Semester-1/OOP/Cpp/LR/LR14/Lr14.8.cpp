#include <iostream>
#include <string>

// Приклад 1: Клас для роботи з відстанню без explicit
class Distance {
private:
    int meters;
    
public:
    // Конструктор без explicit - дозволяє неявне перетворення
    Distance(int m) : meters(m) {
        std::cout << "Конструктор Distance викликаний з " << m << " метрів" << std::endl;
    }
    
    int getMeters() const { return meters; }
};

// Приклад 2: Аналогічний клас, але з explicit
class SafeDistance {
private:
    int meters;
    
public:
    // Конструктор з explicit - забороняє неявне перетворення
    explicit SafeDistance(int m) : meters(m) {
        std::cout << "Конструктор SafeDistance викликаний з " << m << " метрів" << std::endl;
    }
    
    int getMeters() const { return meters; }
};

// Приклад 3: Клас для роботи з файлами
class FileWrapper {
private:
    std::string filename;
    bool isOpen;
    
public:
    // Конструктор без explicit - потенційно небезпечний, бо може призвести
    // до випадкового відкриття файлу через неявне перетворення
    FileWrapper(const std::string& name) : filename(name), isOpen(true) {
        std::cout << "Відкриваємо файл: " << filename << std::endl;
        // У реальному коді тут мало б бути відкриття файлу
    }
    
    ~FileWrapper() {
        if (isOpen) {
            std::cout << "Закриваємо файл: " << filename << std::endl;
            // У реальному коді тут мало б бути закриття файлу
        }
    }
};

// Функції для демонстрації відмінностей
void processDistance(Distance d) {
    std::cout << "Обробка відстані " << d.getMeters() << " метрів" << std::endl;
}

void processSafeDistance(SafeDistance sd) {
    std::cout << "Обробка безпечної відстані " << sd.getMeters() << " метрів" << std::endl;
}

void processFile(FileWrapper fw) {
    std::cout << "Обробка файлу" << std::endl;
}

int main() {
    // Приклад 1: Неявне перетворення без explicit
    std::cout << "\n=== Приклад без explicit ===\n";
    
    Distance d1(100);               // Пряма ініціалізація - очікувано
    Distance d2 = 200;              // Неявне перетворення int в Distance
    processDistance(300);           // Неявне перетворення при виклику функції
    
    // Приклад 2: explicit запобігає неявному перетворенню
    std::cout << "\n=== Приклад з explicit ===\n";
    
    SafeDistance sd1(100);          // Пряма ініціалізація - працює
    
    // Наступні рядки НЕ скомпілюються через explicit:
    // SafeDistance sd2 = 200;      // Помилка: неявне перетворення заборонено
    // processSafeDistance(300);    // Помилка: неявне перетворення заборонено
    
    // Приклад 3: Небезпека неявного перетворення в керуванні ресурсами
    std::cout << "\n=== Приклад з керуванням ресурсами ===\n";
    
    std::string path = "test.txt";
    processFile(path);              // Неявно створює FileWrapper і відкриває файл!
    std::cout << "(Файл закривається при виході з функції)" << std::endl;
    
    std::cout << "\n=== Чому explicit корисний ===\n";
    std::cout << "1. Запобігає випадковим неявним перетворенням типів\n";
    std::cout << "2. Робить код більш чітким і зрозумілим\n";
    std::cout << "3. Уникає несподіваного захоплення ресурсів\n";
    std::cout << "4. Зменшує ймовірність помилкових викликів функцій\n";
    std::cout << "5. Покращує безпеку типів у програмі\n";
    
    return 0;
}