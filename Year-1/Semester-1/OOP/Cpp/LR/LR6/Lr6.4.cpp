#include <iostream>
#include <cmath>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    void show() { cout << "(" << x << ", " << y << ")"; }
    
    // Перевантаження оператора "менше" (<)
    // Порівнюємо за відстанню від початку координат
    bool operator<(const coord &obj) {
        double dist1 = sqrt(x*x + y*y);
        double dist2 = sqrt(obj.x*obj.x + obj.y*obj.y);
        return dist1 < dist2;
    }
    
    // Перевантаження оператора "більше" (>)
    bool operator>(const coord &obj) {
        double dist1 = sqrt(x*x + y*y);
        double dist2 = sqrt(obj.x*obj.x + obj.y*obj.y);
        return dist1 > dist2;
    }
    
    // Додаткові оператори для повноти
    bool operator==(const coord &obj) {
        return (x == obj.x) && (y == obj.y);
    }
    
    bool operator<=(const coord &obj) {
        return (*this < obj) || (*this == obj);
    }
    
    bool operator>=(const coord &obj) {
        return (*this > obj) || (*this == obj);
    }
    
    bool operator!=(const coord &obj) {
        return !(*this == obj);
    }
    
    // Допоміжна функція для отримання відстані
    double distance() const {
        return sqrt(x*x + y*y);
    }
};

int main() {
    cout << "ДЕМОНСТРАЦІЯ ОПЕРАТОРІВ ВІДНОШЕННЯ ДЛЯ КЛАСУ coord:\n";
    cout << "==================================================\n\n";
    
    coord obj1(3, 4);    // відстань = 5
    coord obj2(6, 8);    // відстань = 10
    coord obj3(0, 5);    // відстань = 5
    coord obj4(3, 4);    // відстань = 5 (як obj1)
    
    cout << "Створені об'єкти:\n";
    cout << "obj1: "; obj1.show(); cout << " (відстань: " << obj1.distance() << ")\n";
    cout << "obj2: "; obj2.show(); cout << " (відстань: " << obj2.distance() << ")\n";
    cout << "obj3: "; obj3.show(); cout << " (відстань: " << obj3.distance() << ")\n";
    cout << "obj4: "; obj4.show(); cout << " (відстань: " << obj4.distance() << ")\n\n";
    
    cout << "Порівняння за допомогою оператора '<':\n";
    cout << "obj1 < obj2: " << (obj1 < obj2 ? "true" : "false") << "\n";
    cout << "obj2 < obj1: " << (obj2 < obj1 ? "true" : "false") << "\n";
    cout << "obj1 < obj3: " << (obj1 < obj3 ? "true" : "false") << "\n";
    cout << "obj1 < obj4: " << (obj1 < obj4 ? "true" : "false") << "\n\n";
    
    cout << "Порівняння за допомогою оператора '>':\n";
    cout << "obj1 > obj2: " << (obj1 > obj2 ? "true" : "false") << "\n";
    cout << "obj2 > obj1: " << (obj2 > obj1 ? "true" : "false") << "\n";
    cout << "obj1 > obj3: " << (obj1 > obj3 ? "true" : "false") << "\n";
    cout << "obj1 > obj4: " << (obj1 > obj4 ? "true" : "false") << "\n\n";
    
    cout << "Додаткові оператори:\n";
    cout << "obj1 == obj4: " << (obj1 == obj4 ? "true" : "false") << "\n";
    cout << "obj1 != obj4: " << (obj1 != obj4 ? "true" : "false") << "\n";
    cout << "obj1 <= obj3: " << (obj1 <= obj3 ? "true" : "false") << "\n";
    cout << "obj1 >= obj3: " << (obj1 >= obj3 ? "true" : "false") << "\n\n";
    
    // Демонстрація використання в умовних конструкціях
    cout << "Практичне використання:\n";
    if(obj1 < obj2) {
        cout << "obj1 ближче до початку координат, ніж obj2\n";
    }
    
    if(obj1 == obj4) {
        cout << "obj1 і obj4 знаходяться на однаковій відстані від початку\n";
    }
    
    // Сортування масиву координат
    coord points[] = {coord(1,1), coord(5,5), coord(2,1), coord(0,3)};
    int n = 4;
    
    cout << "\nСортування масиву координат за відстанню:\n";
    cout << "До сортування: ";
    for(int i = 0; i < n; i++) {
        points[i].show();
        cout << " ";
    }
    cout << "\n";
    
    // Простий bubble sort
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < n-i-1; j++) {
            if(points[j] > points[j+1]) {
                coord temp = points[j];
                points[j] = points[j+1];
                points[j+1] = temp;
            }
        }
    }
    
    cout << "Після сортування: ";
    for(int i = 0; i < n; i++) {
        points[i].show();
        cout << " ";
    }
    cout << "\n";
    
    return 0;
}