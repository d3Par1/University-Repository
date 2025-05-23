#include <iostream>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    void show() { cout << "(" << x << ", " << y << ")"; }
    
    // Оголошення дружніх функцій
    friend coord operator-(const coord &obj1, const coord &obj2);  // бінарний мінус
    friend coord operator-(const coord &obj);                       // унарний мінус
    friend coord operator/(const coord &obj1, const coord &obj2);   // ділення
    friend coord operator--(coord &obj);                           // префіксний декремент
    friend coord operator--(coord &obj, int);                      // постфіксний декремент
    
    // Допоміжний оператор присвоєння
    coord& operator=(const coord &obj) {
        x = obj.x;
        y = obj.y;
        return *this;
    }
};

// Реалізація дружніх функцій

// Бінарний оператор мінус (віднімання)
coord operator-(const coord &obj1, const coord &obj2) {
    coord temp;
    temp.x = obj1.x - obj2.x;
    temp.y = obj1.y - obj2.y;
    return temp;
}

// Унарний оператор мінус (зміна знаку)
coord operator-(const coord &obj) {
    coord temp;
    temp.x = -obj.x;
    temp.y = -obj.y;
    return temp;
}

// Оператор ділення
coord operator/(const coord &obj1, const coord &obj2) {
    coord temp;
    if(obj2.x != 0 && obj2.y != 0) {
        temp.x = obj1.x / obj2.x;
        temp.y = obj1.y / obj2.y;
    } else {
        cout << "Помилка: ділення на нуль!\n";
        temp.x = 0;
        temp.y = 0;
    }
    return temp;
}

// Префіксний декремент
coord operator--(coord &obj) {
    --obj.x;
    --obj.y;
    return obj;
}

// Постфіксний декремент
coord operator--(coord &obj, int) {
    coord temp = obj;
    --obj.x;
    --obj.y;
    return temp;
}

int main() {
    cout << "ДЕМОНСТРАЦІЯ ДРУЖНІХ ОПЕРАТОР-ФУНКЦІЙ:\n";
    cout << "======================================\n\n";
    
    coord obj1(20, 30);
    coord obj2(4, 6);
    coord obj3(-8, 12);
    coord result;
    
    cout << "Створені об'єкти:\n";
    cout << "obj1: "; obj1.show(); cout << "\n";
    cout << "obj2: "; obj2.show(); cout << "\n";
    cout << "obj3: "; obj3.show(); cout << "\n\n";
    
    // Демонстрація бінарного мінуса
    cout << "1. БІНАРНИЙ ОПЕРАТОР МІНУС (дружня функція):\n";
    cout << "============================================\n";
    
    result = obj1 - obj2;
    cout << "obj1 - obj2 = "; result.show(); cout << "\n";
    
    result = obj2 - obj1;
    cout << "obj2 - obj1 = "; result.show(); cout << "\n";
    
    result = obj1 - obj3;
    cout << "obj1 - obj3 = "; result.show(); cout << "\n\n";
    
    // Демонстрація унарного мінуса
    cout << "2. УНАРНИЙ ОПЕРАТОР МІНУС (дружня функція):\n";
    cout << "===========================================\n";
    
    result = -obj1;
    cout << "-obj1 = "; result.show(); cout << "\n";
    
    result = -obj3;
    cout << "-obj3 = "; result.show(); cout << "\n";
    
    result = -(-obj3);  // подвійне застосування
    cout << "-(-obj3) = "; result.show(); cout << "\n\n";
    
    // Демонстрація оператора ділення
    cout << "3. ОПЕРАТОР ДІЛЕННЯ (дружня функція):\n";
    cout << "=====================================\n";
    
    result = obj1 / obj2;
    cout << "obj1 / obj2 = "; result.show(); cout << "\n";
    
    coord obj4(15, 25);
    coord obj5(3, 5);
    result = obj4 / obj5;
    cout << "obj4(15,25) / obj5(3,5) = "; result.show(); cout << "\n";
    
    // Тест ділення на нуль
    coord zero(0, 2);
    cout << "\nТест ділення на нуль:\n";
    result = obj1 / zero;
    cout << "obj1 / (0,2) = "; result.show(); cout << "\n\n";
    
    // Демонстрація префіксного декремента
    cout << "4. ПРЕФІКСНИЙ ДЕКРЕМЕНТ (дружня функція):\n";
    cout << "=========================================\n";
    
    coord obj6(10, 15);
    cout << "obj6 до операції: "; obj6.show(); cout << "\n";
    
    result = --obj6;
    cout << "result = --obj6: "; result.show(); cout << "\n";
    cout << "obj6 після операції: "; obj6.show(); cout << "\n";
    cout << "Результат і об'єкт однакові (префіксна форма)\n\n";
    
    // Демонстрація постфіксного декремента
    cout << "5. ПОСТФІКСНИЙ ДЕКРЕМЕНТ (дружня функція):\n";
    cout << "==========================================\n";
    
    coord obj7(10, 15);
    cout << "obj7 до операції: "; obj7.show(); cout << "\n";
    
    result = obj7--;
    cout << "result = obj7--: "; result.show(); cout << "\n";
    cout << "obj7 після операції: "; obj7.show(); cout << "\n";
    cout << "Результат містить старе значення (постфіксна форма)\n\n";
    
    // Комбіноване використання
    cout << "6. КОМБІНОВАНЕ ВИКОРИСТАННЯ:\n";
    cout << "===========================\n";
    
    coord a(20, 30);
    coord b(4, 6);
    coord c(2, 3);
    
    cout << "a: "; a.show(); cout << "\n";
    cout << "b: "; b.show(); cout << "\n";
    cout << "c: "; c.show(); cout << "\n\n";
    
    // Складний вираз
    result = (a - b) / c;
    cout << "(a - b) / c = "; result.show(); cout << "\n";
    
    result = -(a / b);
    cout << "-(a / b) = "; result.show(); cout << "\n";
    
    // Ланцюгові операції
    coord temp1 = a;
    coord temp2 = --temp1;  // префіксний декремент
    result = temp2 - b;
    cout << "(--a) - b = "; result.show(); cout << "\n";
    
    cout << "\n7. ПЕРЕВАГИ ДРУЖНІХ ФУНКЦІЙ:\n";
    cout << "===========================\n";
    cout << "1. Доступ до приватних членів класу\n";
    cout << "2. Можливість природного синтаксису (obj1 - obj2)\n";
    cout << "3. Симетричність операцій\n";
    cout << "4. Більша гнучкість у реалізації\n";
    cout << "5. Можливість роботи з константними об'єктами\n";
    
    return 0;
}