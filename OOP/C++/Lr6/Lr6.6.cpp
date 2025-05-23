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
    
    // Бінарний оператор + (додавання двох координат)
    coord operator+(const coord &obj) {
        coord temp;
        temp.x = x + obj.x;
        temp.y = y + obj.y;
        return temp;
    }
    
    // Унарний оператор + (робить позитивними негативні координати)
    coord operator+() {
        coord temp;
        temp.x = abs(x);  // абсолютне значення x
        temp.y = abs(y);  // абсолютне значення y
        return temp;
    }
    
    // Допоміжні функції для демонстрації
    coord& operator=(const coord &obj) {
        x = obj.x;
        y = obj.y;
        return *this;
    }
    
    bool has_negative() const {
        return x < 0 || y < 0;
    }
};

int main() {
    cout << "ДЕМОНСТРАЦІЯ БІНАРНОГО ТА УНАРНОГО ОПЕРАТОРА ДОДАВАННЯ:\n";
    cout << "=======================================================\n\n";
    
    // Створення об'єктів для тестування
    coord obj1(5, 10);
    coord obj2(3, 7);
    coord obj3(-8, 12);
    coord obj4(-6, -9);
    coord result;
    
    cout << "Створені об'єкти:\n";
    cout << "obj1: "; obj1.show(); cout << "\n";
    cout << "obj2: "; obj2.show(); cout << "\n";
    cout << "obj3: "; obj3.show(); cout << " (має негативну x)\n";
    cout << "obj4: "; obj4.show(); cout << " (має негативні x та y)\n\n";
    
    // Демонстрація бінарного оператора +
    cout << "1. БІНАРНИЙ ОПЕРАТОР + (додавання координат):\n";
    cout << "=============================================\n";
    
    result = obj1 + obj2;
    cout << "obj1 + obj2 = "; result.show(); cout << "\n";
    
    result = obj1 + obj3;
    cout << "obj1 + obj3 = "; result.show(); cout << "\n";
    
    result = obj3 + obj4;
    cout << "obj3 + obj4 = "; result.show(); cout << "\n\n";
    
    // Демонстрація унарного оператора +
    cout << "2. УНАРНИЙ ОПЕРАТОР + (перетворення в позитивні):\n";
    cout << "=================================================\n";
    
    cout << "Об'єкти до застосування унарного +:\n";
    cout << "obj1: "; obj1.show(); 
    cout << (obj1.has_negative() ? " (має негативні)" : " (всі позитивні)") << "\n";
    cout << "obj3: "; obj3.show();
    cout << (obj3.has_negative() ? " (має негативні)" : " (всі позитивні)") << "\n";
    cout << "obj4: "; obj4.show();
    cout << (obj4.has_negative() ? " (має негативні)" : " (всі позитивні)") << "\n\n";
    
    cout << "Результати застосування унарного +:\n";
    result = +obj1;
    cout << "+obj1 = "; result.show(); cout << " (без змін, бо вже позитивні)\n";
    
    result = +obj3;
    cout << "+obj3 = "; result.show(); cout << " (x стало позитивним)\n";
    
    result = +obj4;
    cout << "+obj4 = "; result.show(); cout << " (обидві координати стали позитивними)\n\n";
    
    // Перевірка, що оригінальні об'єкти не змінилися
    cout << "3. ПЕРЕВІРКА НЕЗМІННОСТІ ОРИГІНАЛЬНИХ ОБ'ЄКТІВ:\n";
    cout << "===============================================\n";
    cout << "obj3 після +obj3: "; obj3.show(); cout << " (залишився незмінним)\n";
    cout << "obj4 після +obj4: "; obj4.show(); cout << " (залишився незмінним)\n\n";
    
    // Комбіноване використання
    cout << "4. КОМБІНОВАНЕ ВИКОРИСТАННЯ:\n";
    cout << "===========================\n";
    
    // Спочатку робимо координати позитивними, потім додаємо
    result = (+obj3) + (+obj4);
    cout << "(+obj3) + (+obj4) = "; result.show(); cout << "\n";
    
    // Порівняння з прямим додаванням
    coord direct_sum = obj3 + obj4;
    cout << "obj3 + obj4 = "; direct_sum.show(); cout << " (пряме додавання)\n\n";
    
    // Ланцюгові операції
    cout << "5. ЛАНЦЮГОВІ ОПЕРАЦІЇ:\n";
    cout << "=====================\n";
    
    result = obj1 + obj2 + obj3;
    cout << "obj1 + obj2 + obj3 = "; result.show(); cout << "\n";
    
    result = (+obj3) + (+obj4) + obj1;
    cout << "(+obj3) + (+obj4) + obj1 = "; result.show(); cout << "\n\n";
    
    // Практичний приклад
    cout << "6. ПРАКТИЧНИЙ ПРИКЛАД - ОБРОБКА КООРДИНАТ:\n";
    cout << "=========================================\n";
    
    coord coordinates[] = {coord(-5, 3), coord(2, -7), coord(-1, -4), coord(6, 8)};
    int n = 4;
    
    cout << "Оригінальні координати:\n";
    for(int i = 0; i < n; i++) {
        cout << "coords[" << i << "]: ";
        coordinates[i].show();
        cout << "\n";
    }
    
    cout << "\nКоординати після застосування унарного +:\n";
    for(int i = 0; i < n; i++) {
        result = +coordinates[i];
        cout << "+coords[" << i << "]: ";
        result.show();
        cout << "\n";
    }
    
    // Сума всіх позитивних координат
    coord total_positive;
    for(int i = 0; i < n; i++) {
        total_positive = total_positive + (+coordinates[i]);
    }
    
    cout << "\nСума всіх позитивних координат: ";
    total_positive.show();
    cout << "\n";
    
    return 0;
}