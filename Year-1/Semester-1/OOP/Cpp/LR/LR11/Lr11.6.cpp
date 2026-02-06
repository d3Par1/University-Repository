#include <iostream>
#include <iomanip>

// Базовий клас Dist
class Dist {
protected:
    double distance; // Відстань між двома точками
    
public:
    // Конструктор
    Dist(double d = 0.0) : distance(d) {}
    
    // Функція для встановлення значення відстані
    void setDistance(double d) {
        distance = d;
    }
    
    // Функція для отримання значення відстані
    double getDistance() const {
        return distance;
    }
    
    // Віртуальна функція для розрахунку та виведення часу подорожі
    virtual void trav_time() {
        // Відстань в милях, швидкість = 60 миль/год
        double time = distance / 60.0;
        
        // Перетворення годин на години та хвилини
        int hours = static_cast<int>(time);
        int minutes = static_cast<int>((time - hours) * 60);
        
        std::cout << "Відстань: " << distance << " миль" << std::endl;
        std::cout << "Швидкість: 60 миль/год" << std::endl;
        std::cout << "Час подорожі: " << hours << " год " << minutes << " хв" << std::endl;
    }
    
    // Віртуальний деструктор
    virtual ~Dist() {}
};

// Похідний клас Metric
class Metric : public Dist {
public:
    // Конструктор
    Metric(double d = 0.0) : Dist(d) {}
    
    // Перевизначена функція для розрахунку часу в метричній системі
    void trav_time() override {
        // Відстань в кілометрах, швидкість = 100 км/год
        double time = distance / 100.0;
        
        // Перетворення годин на години та хвилини
        int hours = static_cast<int>(time);
        int minutes = static_cast<int>((time - hours) * 60);
        
        std::cout << "Відстань: " << distance << " кілометрів" << std::endl;
        std::cout << "Швидкість: 100 км/год" << std::endl;
        std::cout << "Час подорожі: " << hours << " год " << minutes << " хв" << std::endl;
    }
};

int main() {
    // Створюємо об'єкти обох класів
    Dist miles(120.0);     // 120 миль
    Metric kilometers(200.0); // 200 кілометрів
    
    // Виводимо час подорожі для кожної відстані
    std::cout << "Час подорожі в милях:" << std::endl;
    miles.trav_time();
    std::cout << std::endl;
    
    std::cout << "Час подорожі в кілометрах:" << std::endl;
    kilometers.trav_time();
    std::cout << std::endl;
    
    // Демонстрація поліморфізму
    std::cout << "Демонстрація поліморфізму з використанням вказівників на базовий клас:" << std::endl;
    
    Dist* ptr;
    
    ptr = &miles;
    ptr->trav_time();
    std::cout << std::endl;
    
    ptr = &kilometers;
    ptr->trav_time();
    
    return 0;
}