#include <iostream>
using namespace std;

// A base class for various types of vehicle
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

class car : public vehicle {
    int passengers;
public:
    // Конструктор car приймає passengers, wheels, range
    car(int p, int w, int r) : vehicle(w, r) {
        passengers = p;
    }
    
    void show() {
        showv();
        cout << "Passengers: " << passengers << '\n';
    }
};

class truck : public vehicle {
    int loadlimit;
public:
    // Конструктор truck приймає loadlimit, wheels, range
    truck(int l, int w, int r) : vehicle(w, r) {
        loadlimit = l;
    }
    
    void show() {
        showv();
        cout << "Loadlimit: " << loadlimit << '\n';
    }
};

int main() {
    car objc(5, 4, 500);      // 5 passengers, 4 wheels, 500 range
    truck objt(3000, 12, 1200); // 3000 loadlimit, 12 wheels, 1200 range
    
    cout << "Car:\n"; 
    objc.show();
    cout << "\nTruck:\n"; 
    objt.show();
    
    return 0;
}