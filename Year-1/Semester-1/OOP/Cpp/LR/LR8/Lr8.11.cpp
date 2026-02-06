#include <iostream>
using namespace std;

class vehicle {
    int num_wheels;
    int range;
public:
    vehicle(int w, int r) : num_wheels(w), range(r) {}
    void showv() {
        cout << "Wheels: " << num_wheels << '\n';
        cout << "Range: " << range << '\n';
    }
};

enum motor { gas, electric, diesel };

class motorized : public vehicle {
    enum motor mtr;
public:
    motorized(enum motor m, int w, int r) : vehicle(w, r), mtr(m) {}
    void showm() {
        cout << "Motor: ";
        switch (mtr) {
            case gas: cout << "Gas\n"; break;
            case electric: cout << "Electric\n"; break;
            case diesel: cout << "Diesel\n"; break;
        }
    }
};

class road_use : public vehicle {
    int passengers;
public:
    road_use(int p, int w, int r) : vehicle(w, r), passengers(p) {}
    void showr() {
        cout << "Passengers: " << passengers << '\n';
    }
};

enum steering { power, rack_pinion, manual };

class car : public motorized, public road_use {
    enum steering strng;
public:
    car(enum steering s, enum motor m, int p, int w, int r)
        : motorized(m, w, r), road_use(p, w, r), strng(s) {}
    void show() {
        motorized::showv();
        showm();
        showr();
        cout << "Steering: ";
        switch (strng) {
            case power: cout << "Power\n"; break;
            case rack_pinion: cout << "Rack and Pinion\n"; break;
            case manual: cout << "Manual\n"; break;
        }
    }
};

int main() {
    car c(power, gas, 5, 4, 500);
    c.show();
    return 0;
}