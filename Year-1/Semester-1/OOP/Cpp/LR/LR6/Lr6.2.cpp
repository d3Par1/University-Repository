#include <iostream>
#include <cmath>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    
    // НЕКОРЕКТНИЙ оператор % з оригінального коду
    /*
    coord coord::operator%(const coord obj) {
        double i;
        cout << "Enter a number: ";
        cin >> i;
        cout << "root of" << i << "is";
        cout << sqrt(i);
        // НЕ ПОВЕРТАЄ ЗНАЧЕННЯ!
    }
    */
    
    // КОРЕКТНА версія оператора %
    coord operator%(const coord &obj) {
        coord temp;
        if(obj.x != 0 && obj.y != 0) {
            temp.x = x % obj.x;
            temp.y = y % obj.y;
        } else {
            cout << "Ділення на нуль в операторі %!\n";
            temp.x = 0;
            temp.y = 0;
        }
        return temp;
    }
    
    void show() {
        cout << "(" << x << ", " << y << ")\n";
    }
};

int main() {
    cout << "АНАЛІЗ ПОМИЛОК В НЕКОРЕКТНОМУ ОПЕРАТОРІ %:\n";
    cout << "=========================================\n\n";
    
    cout << "1. ВІДСУТНІСТЬ ПОВЕРНЕННЯ ЗНАЧЕННЯ:\n";
    cout << "   - Функція має тип coord, але не повертає жодного об'єкта\n";
    cout << "   - Це призведе до помилки компіляції\n\n";
    
    cout << "2. ЛОГІЧНА НЕВІДПОВІДНІСТЬ:\n";
    cout << "   - Оператор % має обчислювати остачу від ділення\n";
    cout << "   - А функція обчислює квадратний корінь\n";
    cout << "   - Це порушує семантику оператора\n\n";
    
    cout << "3. ПОБІЧНІ ЕФЕКТИ:\n";
    cout << "   - Оператор виводить результат на екран\n";
    cout << "   - Запитує введення від користувача\n";
    cout << "   - Це не є типовою поведінкою арифметичних операторів\n\n";
    
    cout << "4. НЕВИКОРИСТАННЯ ПАРАМЕТРА:\n";
    cout << "   - Параметр obj не використовується в обчисленнях\n";
    cout << "   - Це робить оператор безглуздим\n\n";
    
    cout << "ДЕМОНСТРАЦІЯ КОРЕКТНОГО ОПЕРАТОРА %:\n";
    cout << "=====================================\n";
    
    coord obj1(17, 23);
    coord obj2(5, 7);
    coord result;
    
    cout << "obj1: ";
    obj1.show();
    cout << "obj2: ";
    obj2.show();
    
    result = obj1 % obj2;
    cout << "obj1 % obj2 = ";
    result.show();
    
    return 0;
}