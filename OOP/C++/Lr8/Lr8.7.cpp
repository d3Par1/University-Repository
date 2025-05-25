#include <iostream>
using namespace std;

class vehicle {
    int num_wheels;
    int range;
public:
    vehicle(int w, int r) : num_wheels(w), range(r) {}
    void show() {
        cout << "Wheels: " << num_wheels << '\n';
        cout << "Range: " << range << '\n';
    }
};

class car : public vehicle {
    int passengers;
public:
    car(int p, int w, int r) : vehicle(w, r), passengers(p) {}
    void show() {
        vehicle::show();
        cout << "Passengers: " << passengers << '\n';
    }
};

class truck : public vehicle {
    int loadlimit;
public:
    truck(int l, int w, int r) : vehicle(w, r), loadlimit(l) {}
    void show() {
        vehicle::show();
        cout << "Loadlimit: " << loadlimit << '\n';
    }
};

int main() {
    car objc(5, 4, 500);
    truck objt(3000, 12, 1200);
    cout << "Car:\n";
    objc.show();
    cout << "Truck:\n";
    objt.show();
    return 0;
}