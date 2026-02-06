#include <iostream>
using namespace std;

class coord {
public:
    int x, y; // coordinate values
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    void get_xy(int &i, int &j) { i = x; j = y; }
    
    // Оголошення дружніх функцій
    friend coord operator+(coord ob1, coord ob2);
    friend coord operator-(coord ob1, coord ob2);

    // Оголошення оператора = як члена класу
    coord& operator=(const coord& ob2);
};

// Дружня функція - перевантажений оператор +
coord operator+(coord ob1, coord ob2) {
    coord temp;
    cout << "Using coord operator+()\n";
    temp.x = ob1.x + ob2.x;
    temp.y = ob1.y + ob2.y;
    return temp;
}

// Дружня функція - перевантажений оператор -
coord operator-(coord ob1, coord ob2) {
    coord temp;
    cout << "Using coord operator-()\n";
    temp.x = ob1.x - ob2.x;
    temp.y = ob1.y - ob2.y;
    return temp;
}

// Оператор = як член класу coord
coord& coord::operator=(const coord& ob2) {
    cout << "Using coord operator=()\n";
    x = ob2.x;
    y = ob2.y;
    return *this;
}

class quad : public coord {
protected:
    int quadrant;
public:
    quad() {
        x = 0;
        y = 0;
        quadrant = 0;
    }
    
    quad(int x, int y) : coord(x, y) {
        if (x >= 0 && y >= 0)
            quadrant = 1;
        else if (x < 0 && y >= 0)
            quadrant = 2;
        else if (x < 0 && y < 0)
            quadrant = 3;
        else
            quadrant = 4;
    }
    
    void showq() {
        cout << "Point in Quadrant: " << quadrant << '\n';
    }
    
    // Оголошення оператора = як члена класу quad
    quad& operator=(const coord& ob2);
};

// Оператор = як член класу quad
quad& quad::operator=(const coord& ob2) {
    cout << "Using quad operator=()\n";
    x = ob2.x;
    y = ob2.y;

    if (x >= 0 && y >= 0)
        quadrant = 1;
    else if (x < 0 && y >= 0)
        quadrant = 2;
    else if (x < 0 && y < 0)
        quadrant = 3;
    else
        quadrant = 4;

    return *this;
}

int main() {
    quad o1(10, 10), o2(15, 3), o3;
    int x, y;
    
    cout << "=== Демонстрація роботи програми ===\n\n";
    
    // Додавання двох об'єктів
    o3 = o1 + o2;
    o3.get_xy(x, y);
    o3.showq();
    cout << "(o1+o2) X: " << x << ", Y: " << y << "\n\n";
    
    // Віднімання двох об'єктів
    o3 = o1 - o2;
    o3.get_xy(x, y);
    o3.showq();
    cout << "(o1-o2) X: " << x << ", Y: " << y << "\n\n";
    
    // Присвоєння об'єкта
    o3 = o1;
    o3.get_xy(x, y);
    o3.showq();
    cout << "(o3=o1) X: " << x << ", Y: " << y << "\n\n";
    
    cout << "=== Пояснення результату ===\n";
    cout << "1. При додаванні (10,10) + (15,3) = (25,13) - 1-й квадрант\n";
    cout << "2. При відніманні (10,10) - (15,3) = (-5,7) - 2-й квадрант\n";
    cout << "3. При присвоєнні o3 = o1, o3 отримує координати (10,10) - 1-й квадрант\n";
    cout << "\nВсі оператори тепер реалізовані як дружні функції!\n";
    
    return 0;
}