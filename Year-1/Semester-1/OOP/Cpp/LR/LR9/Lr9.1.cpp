#include <iostream>
#include <string>
using namespace std;

// Базовий клас для літальних апаратів
class Airship {
protected:
    int passengers;      // кількість пасажирів
    double cargo_weight; // вага вантажу
    string weight_unit;  // одиниця виміру ваги (фунти, кілограми, тонни)
    
public:
    Airship(int pass = 0, double cargo = 0.0, string unit = "kg") {
        passengers = pass;
        cargo_weight = cargo;
        weight_unit = unit;
    }
    
    void showBase() {
        cout << "Пасажири: " << passengers << endl;
        cout << "Вантаж: " << cargo_weight << " " << weight_unit << endl;
    }
};

// Похідний клас Літак
class Airplane : public Airship {
private:
    string engine_type;  // тип двигуна (гвинтовий або реактивний)
    double range;        // дальність польоту
    string range_unit;   // одиниця виміру дальності (милі або кілометри)
    
public:
    Airplane(int pass, double cargo, string w_unit, 
             string engine, double rng, string r_unit) 
        : Airship(pass, cargo, w_unit) {
        engine_type = engine;
        range = rng;
        range_unit = r_unit;
    }
    
    void show() {
        cout << "\n=== Літак ===" << endl;
        showBase();
        cout << "Тип двигуна: " << engine_type << endl;
        cout << "Дальність польоту: " << range << " " << range_unit << endl;
    }
};

// Похідний клас Дирижабль
class Balloon : public Airship {
private:
    string gas_type;      // тип газу (водень або гелій)
    double max_altitude;  // максимальна висота підйому
    string alt_unit;      // одиниця виміру висоти (фути або метри)
    
public:
    Balloon(int pass, double cargo, string w_unit,
            string gas, double alt, string a_unit)
        : Airship(pass, cargo, w_unit) {
        gas_type = gas;
        max_altitude = alt;
        alt_unit = a_unit;
    }
    
    void show() {
        cout << "\n=== Дирижабль ===" << endl;
        showBase();
        cout << "Тип газу: " << gas_type << endl;
        cout << "Максимальна висота: " << max_altitude << " " << alt_unit << endl;
    }
};

int main() {
    // Створюємо об'єкт літака
    Airplane plane1(150, 5000, "кг", "реактивний", 8000, "км");
    Airplane plane2(50, 2000, "фунтів", "гвинтовий", 3000, "миль");
    
    // Створюємо об'єкти дирижаблів
    Balloon balloon1(20, 1000, "кг", "гелій", 5000, "метрів");
    Balloon balloon2(30, 2000, "тонн", "водень", 15000, "футів");
    
    // Демонструємо роботу
    cout << "ДЕМОНСТРАЦІЯ ІЄРАРХІЇ КЛАСІВ ЛІТАЛЬНИХ АПАРАТІВ" << endl;
    cout << "================================================" << endl;
    
    plane1.show();
    plane2.show();
    balloon1.show();
    balloon2.show();
    
    return 0;
}