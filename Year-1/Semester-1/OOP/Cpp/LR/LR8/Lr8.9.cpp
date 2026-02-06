#include <iostream>
using namespace std;

class building {
    int floors, rooms;
    double area;
public:
    building(int f, int r, double a) : floors(f), rooms(r), area(a) {}
    void show() {
        cout << "Floors: " << floors << ", Rooms: " << rooms << ", Area: " << area << " sq ft\n";
    }
};

class house : public building {
    int bedrooms, bathrooms;
public:
    house(int f, int r, double a, int bed, int bath) : building(f, r, a), bedrooms(bed), bathrooms(bath) {}
    void show() {
        building::show();
        cout << "Bedrooms: " << bedrooms << ", Bathrooms: " << bathrooms << '\n';
    }
};

class office : public building {
    int fire_extinguishers, phones;
public:
    office(int f, int r, double a, int fe, int p) : building(f, r, a), fire_extinguishers(fe), phones(p) {}
    void show() {
        building::show();
        cout << "Fire Extinguishers: " << fire_extinguishers << ", Phones: " << phones << '\n';
    }
};

int main() {
    house h(2, 5, 2000, 3, 2);
    office o(10, 50, 10000, 5, 20);
    cout << "House:\n";
    h.show();
    cout << "Office:\n";
    o.show();
    return 0;
}