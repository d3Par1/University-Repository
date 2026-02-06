#include <iostream>
using namespace std;

class planet {
protected:
    double distance;
    int revolve;
public:
    planet(double d, int r) : distance(d), revolve(r) {}
};

class earth : public planet {
    double circumference;
public:
    earth(double d, int r) : planet(d, r) {
        circumference = 2 * distance * 3.1416;
    }
    void show() {
        cout << "Distance from sun: " << distance << '\n';
        cout << "Days in orbit: " << revolve << '\n';
        cout << "Circumference of orbit: " << circumference << '\n';
    }
};

int main() {
    earth obj(93000000, 365);
    obj.show();
    return 0;
}