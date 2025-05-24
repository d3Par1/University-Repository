#include <iostream>
using namespace std;

// Базовий клас building
class building {
protected:
    int floors;        // число поверхів
    int rooms;         // число кімнат
    double total_area; // загальна площа кімнат
public:
    building(int f, int r, double a) {
        floors = f;
        rooms = r;
        total_area = a;
    }
    
    void show() {
        cout << "Поверхів: " << floors << endl;
        cout << "Кімнат: " << rooms << endl;
        cout << "Загальна площа: " << total_area << " кв.м" << endl;
    }
};

// Похідний клас house
class house : public building {
    int bathrooms;  // число ванних кімнат
    int bedrooms;   // число спалень
public:
    house(int f, int r, double a, int bath, int bed) 
        : building(f, r, a) {
        bathrooms = bath;
        bedrooms = bed;
    }
    
    void show() {
        cout << "=== Будинок ===" << endl;
        building::show();  // Виклик методу базового класу
        cout << "Ванних кімнат: " << bathrooms << endl;
        cout << "Спалень: " << bedrooms << endl;
    }
};

// Похідний клас office
class office : public building {
    int fire_extinguishers;  // число вогнегасників
    int phones;              // число телефонів
public:
    office(int f, int r, double a, int fe, int ph) 
        : building(f, r, a) {
        fire_extinguishers = fe;
        phones = ph;
    }
    
    void show() {
        cout << "=== Офіс ===" << endl;
        building::show();  // Виклик методу базового класу
        cout << "Вогнегасників: " << fire_extinguishers << endl;
        cout << "Телефонів: " << phones << endl;
    }
};

int main() {
    // Створюємо об'єкт будинку: 2 поверхи, 8 кімнат, 200 кв.м, 2 ванні, 3 спальні
    house myHouse(2, 8, 200.0, 2, 3);
    myHouse.show();
    
    cout << endl;
    
    // Створюємо об'єкт офісу: 5 поверхів, 50 кімнат, 1500 кв.м, 10 вогнегасників, 25 телефонів
    office myOffice(5, 50, 1500.0, 10, 25);
    myOffice.show();
    
    return 0;
}