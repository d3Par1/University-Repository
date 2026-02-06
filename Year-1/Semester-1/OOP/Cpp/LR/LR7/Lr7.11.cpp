#include <iostream>
using namespace std;

// A base class for various types of vehicles
class vehicle {
    int num_wheels;
    int range;
public:
    vehicle(int w, int r) {
        num_wheels = w;
        range = r;
    }
    
    void showv() {
        cout << "Wheels: " << num_wheels << '\n';
        cout << "Range: " << range << '\n';
    }
};

enum motor { gas, electric, diesel };

// Віртуальне наслідування для вирішення проблеми ромба
class motorized : virtual public vehicle {
    enum motor mtr;
public:
    motorized(enum motor m, int w, int r) : vehicle(w, r) {
        mtr = m;
    }
    
    void showm() {
        cout << "Motor: ";
        switch (mtr) {
            case gas:
                cout << "Gas\n";
                break;
            case electric:
                cout << "Electric\n";
                break;
            case diesel:
                cout << "Diesel\n";
                break;
        }
    }
};

// Віртуальне наслідування для вирішення проблеми ромба
class road_use : virtual public vehicle {
    int passengers;
public:
    road_use(int p, int w, int r) : vehicle(w, r) {
        passengers = p;
    }
    
    void showr() {
        cout << "Passengers: " << passengers << '\n';
    }
};

enum steering { power, rack_pinion, manual };

class car : public motorized, public road_use {
    enum steering strng;
public:
    // Виправлений конструктор - vehicle викликається лише один раз
    car(enum steering s, enum motor m, int w, int r, int p) :
        vehicle(w, r),  // Прямий виклик конструктора vehicle
        road_use(p, w, r), 
        motorized(m, w, r) {
        strng = s;
    }
    
    void show() {
        showv();
        showr();
        showm();
        cout << "Steering: ";
        switch (strng) {
            case power:
                cout << "Power\n";
                break;
            case rack_pinion:
                cout << "Rack and Pinion\n";
                break;
            case manual:
                cout << "Manual\n";
                break;
        }
    }
};

int main() {
    car c(power, gas, 4, 500, 5);
    c.show();
    
    cout << "\n--- Пояснення помилки та попереджень ---\n";
    cout << "Помилка була пов'язана з 'проблемою ромба' при множинному наслідуванні.\n";
    cout << "Класи motorized та road_use обидва наслідують vehicle,\n";
    cout << "а клас car наслідує обидва ці класи.\n";
    cout << "Рішення: використання віртуального наслідування (virtual public).\n\n";
    
    cout << "Попередження про switch можуть з'являтися через:\n";
    cout << "1. Використання enum без явного приведення типів\n";
    cout << "2. Відсутність default case в switch\n";
    cout << "3. Старі компілятори можуть вимагати enum class замість простого enum\n";
    
    return 0;
}