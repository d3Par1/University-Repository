#include <iostream>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    void show() { cout << "(" << x << ", " << y << ")\n"; }
    
    // Стандартний оператор + (повертає coord)
    coord operator+(const coord &obj) {
        coord temp;
        temp.x = x + obj.x;
        temp.y = y + obj.y;
        return temp;
    }
    
    // Експеримент 1: оператор - повертає int
    int operator-(const coord &obj) {
        return (x - obj.x) + (y - obj.y);  // Сума різниць
    }
    
    // Експеримент 2: оператор * повертає double
    double operator*(const coord &obj) {
        return double(x * obj.x + y * obj.y);  // Скалярний добуток
    }
    
    // Експеримент 3: оператор / повертає void
    void operator/(const coord &obj) {
        if(obj.x != 0 && obj.y != 0) {
            cout << "Результат ділення: (" << x/obj.x << ", " << y/obj.y << ")\n";
        } else {
            cout << "Ділення на нуль!\n";
        }
    }
    
    // Експеримент 4: оператор % повертає bool
    bool operator%(const coord &obj) {
        return (x % 2 == obj.x % 2) && (y % 2 == obj.y % 2);
    }
};

int main() {
    cout << "ЕКСПЕРИМЕНТИ З РІЗНИМИ ТИПАМИ ПОВЕРНЕННЯ:\n";
    cout << "==========================================\n\n";
    
    coord obj1(10, 15);
    coord obj2(3, 5);
    
    cout << "obj1: ";
    obj1.show();
    cout << "obj2: ";
    obj2.show();
    cout << "\n";
    
    // Стандартний оператор +
    cout << "1. СТАНДАРТНИЙ ОПЕРАТОР + (повертає coord):\n";
    coord result_coord = obj1 + obj2;
    cout << "obj1 + obj2 = ";
    result_coord.show();
    cout << "✓ Нормальна робота, можна ланцюгові операції\n\n";
    
    // Експеримент 1: int
    cout << "2. ОПЕРАТОР - ПОВЕРТАЄ int:\n";
    int result_int = obj1 - obj2;
    cout << "obj1 - obj2 = " << result_int << "\n";
    cout << "⚠ Втрачається інформація про структуру об'єкта\n";
    // Наступний рядок НЕ СКОМПІЛЮЄТЬСЯ:
    // coord chain = (obj1 - obj2) + obj1;  // ПОМИЛКА!
    cout << "✗ Неможливі ланцюгові операції з coord\n\n";
    
    // Експеримент 2: double
    cout << "3. ОПЕРАТОР * ПОВЕРТАЄ double:\n";
    double result_double = obj1 * obj2;
    cout << "obj1 * obj2 = " << result_double << "\n";
    cout << "⚠ Корисно для математичних операцій, але втрачається тип\n\n";
    
    // Експеримент 3: void
    cout << "4. ОПЕРАТОР / ПОВЕРТАЄ void:\n";
    obj1 / obj2;  // Нічого не можна присвоїти
    cout << "✗ Неможливо зберегти результат\n";
    cout << "✗ Порушується принцип функціональності операторів\n\n";
    
    // Експеримент 4: bool
    cout << "5. ОПЕРАТОР % ПОВЕРТАЄ bool:\n";
    bool result_bool = obj1 % obj2;
    cout << "obj1 % obj2 = " << (result_bool ? "true" : "false") << "\n";
    cout << "⚠ Корисно для порівнянь, але втрачається оригінальна семантика\n\n";
    
    cout << "ВИСНОВКИ:\n";
    cout << "=========\n";
    cout << "1. Зміна типу повернення порушує очікувану семантику операторів\n";
    cout << "2. Ускладнює або унеможливлює ланцюгові операції\n";
    cout << "3. Може призвести до плутанини для користувачів класу\n";
    cout << "4. Деякі типи (void) роблять оператор менш функціональним\n";
    cout << "5. Краще дотримуватися стандартних конвенцій для операторів\n";
    
    return 0;
}