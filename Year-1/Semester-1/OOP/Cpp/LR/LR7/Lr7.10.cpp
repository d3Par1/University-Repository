#include <iostream>
using namespace std;

class planet {
protected:
    double distance;  // miles from the sun
    int revolve;      // in days
public:
    planet(double d, int r) { 
        distance = d; 
        revolve = r; 
    }
};

class earth : public planet {
    double circumference;  // circumference (окружність) of orbit
public:
    /*
    Create earth(double d, int r). Have it pass the distance and days of revolution
    back to planet. Have it compute the circumference of the orbit. 
    (Hint: circumference = 2r*3.1416.)
    Create a function called show() that displays the information.
    */
    
    // Конструктор earth передає відстань та дні обертання до planet
    // та обчислює довжину орбіти
    earth(double d, int r) : planet(d, r) {
        // Довжина орбіти = 2 * π * радіус (відстань від сонця)
        circumference = 2 * distance * 3.1416;
    }
    
    // Функція show() відображає інформацію
    void show() {
        cout << "=== Планета Земля ===" << endl;
        cout << "Відстань від Сонця: " << distance << " миль" << endl;
        cout << "Період обертання: " << revolve << " днів" << endl;
        cout << "Довжина орбіти: " << circumference << " миль" << endl;
    }
};

int main() {
    earth obj(93000000, 365);
    obj.show();
    return 0;
}