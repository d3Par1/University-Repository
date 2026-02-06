#include <iostream>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    
    // Перевантаження оператора множення
    coord operator*(const coord &obj) {
        coord temp;
        temp.x = x * obj.x;
        temp.y = y * obj.y;
        return temp;
    }
    
    // Перевантаження оператора ділення
    coord operator/(const coord &obj) {
        coord temp;
        if(obj.x != 0 && obj.y != 0) {
            temp.x = x / obj.x;
            temp.y = y / obj.y;
        } else {
            cout << "Ділення на нуль!\n";
            temp.x = 0;
            temp.y = 0;
        }
        return temp;
    }
    
    // Функція для виведення координат
    void show() {
        cout << "(" << x << ", " << y << ")\n";
    }
};

int main() {
    coord obj1(10, 20);
    coord obj2(2, 4);
    coord obj3;
    
    cout << "obj1: ";
    obj1.show();
    cout << "obj2: ";
    obj2.show();
    
    // Демонстрація множення
    obj3 = obj1 * obj2;
    cout << "obj1 * obj2 = ";
    obj3.show();
    
    // Демонстрація ділення
    obj3 = obj1 / obj2;
    cout << "obj1 / obj2 = ";
    obj3.show();
    
    // Тест ділення на нуль
    coord zero(0, 0);
    cout << "Тест ділення на нуль:\n";
    obj3 = obj1 / zero;
    cout << "obj1 / zero = ";
    obj3.show();
    
    return 0;
}