#include <iostream>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    void show() { cout << "(" << x << ", " << y << ")"; }
    
    // Префіксний оператор мінус (декремент)
    // --obj - спочатку зменшує, потім повертає нове значення
    coord& operator--() {
        --x;
        --y;
        return *this;
    }
    
    // Постфіксний оператор мінус (декремент) 
    // obj-- - повертає старе значення, потім зменшує
    coord operator--(int) {
        coord temp = *this;  // зберігаємо поточне значення
        --x;
        --y;
        return temp;  // повертаємо старе значення
    }
    
    // Унарний мінус (змінює знак координат)
    coord operator-() {
        coord temp;
        temp.x = -x;
        temp.y = -y;
        return temp;
    }
    
    // Бінарний мінус (віднімання двох координат)
    coord operator-(const coord &obj) {
        coord temp;
        temp.x = x - obj.x;
        temp.y = y - obj.y;
        return temp;
    }
    
    // Допоміжний оператор присвоєння
    coord& operator=(const coord &obj) {
        x = obj.x;
        y = obj.y;
        return *this;
    }
};

int main() {
    cout << "ДЕМОНСТРАЦІЯ РІЗНИХ ФОРМ ОПЕРАТОРА МІНУС:\n";
    cout << "=========================================\n\n";
    
    coord obj1(10, 15);
    coord obj2(3, 5);
    coord result;
    
    cout << "Початкові значення:\n";
    cout << "obj1: "; obj1.show(); cout << "\n";
    cout << "obj2: "; obj2.show(); cout << "\n\n";
    
    // 1. Унарний мінус
    cout << "1. УНАРНИЙ МІНУС (зміна знаку):\n";
    result = -obj1;
    cout << "-obj1 = "; result.show(); cout << "\n";
    cout << "obj1 залишається: "; obj1.show(); cout << "\n\n";
    
    // 2. Бінарний мінус
    cout << "2. БІНАРНИЙ МІНУС (віднімання):\n";
    result = obj1 - obj2;
    cout << "obj1 - obj2 = "; result.show(); cout << "\n\n";
    
    // 3. Префіксний декремент
    cout << "3. ПРЕФІКСНИЙ ДЕКРЕМЕНТ (--obj):\n";
    coord obj3(10, 15);
    cout << "obj3 до операції: "; obj3.show(); cout << "\n";
    
    result = --obj3;  // спочатку зменшує obj3, потім присвоює result
    cout << "result = --obj3: "; result.show(); cout << "\n";
    cout << "obj3 після операції: "; obj3.show(); cout << "\n";
    cout << "Зверніть увагу: obj3 і result однакові!\n\n";
    
    // 4. Постфіксний декремент  
    cout << "4. ПОСТФІКСНИЙ ДЕКРЕМЕНТ (obj--):\n";
    coord obj4(10, 15);
    cout << "obj4 до операції: "; obj4.show(); cout << "\n";
    
    result = obj4--;  // спочатку присвоює result старе значення obj4, потім зменшує obj4
    cout << "result = obj4--: "; result.show(); cout << "\n";
    cout << "obj4 після операції: "; obj4.show(); cout << "\n";
    cout << "Зверніть увагу: result має старе значення obj4!\n\n";
    
    // 5. Порівняння префіксної та постфіксної форм
    cout << "5. ПОРІВНЯННЯ ПРЕФІКСНОЇ ТА ПОСТФІКСНОЇ ФОРМ:\n";
    coord obj5(5, 8);
    coord obj6(5, 8);
    
    cout << "Початкові значення:\n";
    cout << "obj5: "; obj5.show(); cout << "\n";
    cout << "obj6: "; obj6.show(); cout << "\n\n";
    
    cout << "Префіксна форма:\n";
    cout << "Значення --obj5: "; (--obj5).show(); cout << "\n";
    cout << "obj5 після операції: "; obj5.show(); cout << "\n\n";
    
    cout << "Постфіксна форма:\n";
    cout << "Значення obj6--: "; (obj6--).show(); cout << "\n";
    cout << "obj6 після операції: "; obj6.show(); cout << "\n\n";
    
    // 6. Використання в циклах
    cout << "6. ВИКОРИСТАННЯ В ЦИКЛАХ:\n";
    coord counter(3, 3);
    
    cout << "Цикл з префіксним декрементом:\n";
    coord temp_counter = counter;
    int x;
    int y;
    while(temp_counter.get_xy(x, y), x > 0) {
        cout << "Поточне значення: "; temp_counter.show(); cout << "\n";
        --temp_counter;
    }
    
    cout << "\nЦикл з постфіксним декрементом:\n";
    temp_counter = counter;
    while(temp_counter.get_xy(x, y), x > 0) {
        cout << "Поточне значення: "; temp_counter.show(); cout << "\n";
        temp_counter--;
    }
    
    return 0;
}