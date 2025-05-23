#include <iostream>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    void show() { cout << "(" << x << ", " << y << ")"; }
    
    // Метод класу для obj * int
    coord operator*(int multiplier) {
        coord temp;
        temp.x = x * multiplier;
        temp.y = y * multiplier;
        return temp;
    }
    
    // Дружня функція для int * obj
    friend coord operator*(int multiplier, const coord &obj);
    
    // Допоміжні оператори
    coord& operator=(const coord &obj) {
        x = obj.x;
        y = obj.y;
        return *this;
    }
    
    bool operator==(const coord &obj) {
        return (x == obj.x) && (y == obj.y);
    }
};

// Реалізація дружньої функції для int * obj
coord operator*(int multiplier, const coord &obj) {
    coord temp;
    temp.x = obj.x * multiplier;
    temp.y = obj.y * multiplier;
    return temp;
}

int main() {
    cout << "ДЕМОНСТРАЦІЯ МНОЖЕННЯ КООРДИНАТ НА ЦІЛЕ ЧИСЛО:\n";
    cout << "==============================================\n\n";
    
    coord obj1(3, 5);
    coord obj2(-2, 7);
    coord result;
    
    cout << "Створені об'єкти:\n";
    cout << "obj1: "; obj1.show(); cout << "\n";
    cout << "obj2: "; obj2.show(); cout << "\n\n";
    
    // Демонстрація obj * int
    cout << "1. МНОЖЕННЯ ОБ'ЄКТА НА ЧИСЛО (obj * int):\n";
    cout << "========================================\n";
    
    result = obj1 * 3;
    cout << "obj1 * 3 = "; result.show(); cout << "\n";
    
    result = obj1 * 0;
    cout << "obj1 * 0 = "; result.show(); cout << "\n";
    
    result = obj1 * (-2);
    cout << "obj1 * (-2) = "; result.show(); cout << "\n";
    
    result = obj2 * 4;
    cout << "obj2 * 4 = "; result.show(); cout << "\n\n";
    
    // Демонстрація int * obj
    cout << "2. МНОЖЕННЯ ЧИСЛА НА ОБ'ЄКТ (int * obj):\n";
    cout << "========================================\n";
    
    result = 3 * obj1;
    cout << "3 * obj1 = "; result.show(); cout << "\n";
    
    result = 0 * obj1;
    cout << "0 * obj1 = "; result.show(); cout << "\n";
    
    result = (-2) * obj1;
    cout << "(-2) * obj1 = "; result.show(); cout << "\n";
    
    result = 4 * obj2;
    cout << "4 * obj2 = "; result.show(); cout << "\n\n";
    
    // Перевірка комутативності
    cout << "3. ПЕРЕВІРКА КОМУТАТИВНОСТІ:\n";
    cout << "============================\n";
    
    coord result1 = obj1 * 5;
    coord result2 = 5 * obj1;
    
    cout << "obj1 * 5 = "; result1.show(); cout << "\n";
    cout << "5 * obj1 = "; result2.show(); cout << "\n";
    cout << "Результати однакові: " << (result1 == result2 ? "Так" : "Ні") << "\n\n";
    
    result1 = obj2 * (-3);
    result2 = (-3) * obj2;
    
    cout << "obj2 * (-3) = "; result1.show(); cout << "\n";
    cout << "(-3) * obj2 = "; result2.show(); cout << "\n";
    cout << "Результати однакові: " << (result1 == result2 ? "Так" : "Ні") << "\n\n";
    
    // Ланцюгові операції
    cout << "4. ЛАНЦЮГОВІ ОПЕРАЦІЇ:\n";
    cout << "=====================\n";
    
    result = (obj1 * 2) * 3;  // (obj1 * 2) потім * 3
    cout << "(obj1 * 2) * 3 = "; result.show(); cout << "\n";
    
    coord temp = 2 * obj1;
    result = 3 * temp;
    cout << "3 * (2 * obj1) = "; result.show(); cout << "\n";
    
    // Прямий результат для порівняння
    result = obj1 * 6;
    cout << "obj1 * 6 = "; result.show(); cout << " (для порівняння)\n\n";
    
    // Практичні приклади
    cout << "5. ПРАКТИЧНІ ПРИКЛАДИ:\n";
    cout << "=====================\n";
    
    // Масштабування координат
    coord point(2, 3);
    cout << "Оригінальна точка: "; point.show(); cout << "\n";
    
    cout << "Масштабування в 2 рази: "; (point * 2).show(); cout << "\n";
    cout << "Масштабування в 5 разів: "; (5 * point).show(); cout << "\n";
    cout << "Зменшення вдвічі: "; (point * 0).show(); cout << " (множення на 0)\n\n";
    
    // Обертання знаку
    cout << "Зміна напрямку (множення на -1):\n";
    result = point * (-1);
    cout << "point * (-1) = "; result.show(); cout << "\n";
    result = (-1) * point;
    cout << "(-1) * point = "; result.show(); cout << "\n\n";
    
    // Робота з масивом точок
    cout << "6. РОБОТА З МАСИВОМ ТОЧОК:\n";
    cout << "==========================\n";
    
    coord points[] = {coord(1, 2), coord(3, 4), coord(-1, 5), coord(2, -3)};
    int n = 4;
    int scale_factor = 3;
    
    cout << "Оригінальні точки:\n";
    for(int i = 0; i < n; i++) {
        cout << "points[" << i << "]: ";
        points[i].show();
        cout << "\n";
    }
    
    cout << "\nТочки після масштабування на " << scale_factor << ":\n";
    for(int i = 0; i < n; i++) {
        result = points[i] * scale_factor;
        cout << "points[" << i << "] * " << scale_factor << ": ";
        result.show();
        cout << "\n";
    }
    
    cout << "\nТочки після масштабування " << scale_factor << " * point:\n";
    for(int i = 0; i < n; i++) {
        result = scale_factor * points[i];
        cout << scale_factor << " * points[" << i << "]: ";
        result.show();
        cout << "\n";
    }
    
    cout << "\n7. ЧОМУ ПОТРІБНІ ДВІ РІЗНІ РЕАЛІЗАЦІЇ:\n";
    cout << "=====================================\n";
    cout << "1. obj * int - реалізується як метод класу\n";
    cout << "2. int * obj - реалізується як дружня функція\n";
    cout << "3. Це забезпечує природний синтаксис для обох варіантів\n";
    cout << "4. Без дружньої функції int * obj не працювало б\n";
    cout << "5. Комутативність операції множення зберігається\n";
    
    return 0;
}