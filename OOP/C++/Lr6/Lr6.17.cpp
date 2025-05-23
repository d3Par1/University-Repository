#include <iostream>
using namespace std;

class three_d {
    int x, y, z;
public:
    three_d(int i, int j, int k) { x = i; y = j; z = k; }
    three_d() { x = 0; y = 0; z = 0; }
    void get(int &i, int &j, int &k) { i = x; j = y; k = z; }
    void show() { cout << "(" << x << ", " << y << ", " << z << ")"; }
    
    // Оператори з параметрами-посиланнями
    three_d& operator+(const three_d &obj) {
        static three_d temp;  // Статичний об'єкт для повернення посилання
        temp.x = x + obj.x;
        temp.y = y + obj.y;
        temp.z = z + obj.z;
        return temp;
    }
    
    three_d& operator-(const three_d &obj) {
        static three_d temp;
        temp.x = x - obj.x;
        temp.y = y - obj.y;
        temp.z = z - obj.z;
        return temp;
    }
    
    // Оператори дорівнює, не дорівнює
    bool operator==(const three_d &obj) const {
        return (x == obj.x) && (y == obj.y) && (z == obj.z);
    }
    
    bool operator!=(const three_d &obj) const {
        return !(*this == obj);
    }
    
    // Логічне АБО (перевіряє, чи хоча б одна координата не нульова)
    bool operator||(const three_d &obj) const {
        return (x != 0 || y != 0 || z != 0) || (obj.x != 0 || obj.y != 0 || obj.z != 0);
    }
    
    // Дружні функції для інкремента і декремента
    friend three_d& operator++(three_d &obj);      // префіксний інкремент
    friend three_d& operator--(three_d &obj);      // префіксний декремент
    
    // Дружні функції для змішаних операцій obj+int і int+obj
    friend three_d operator+(const three_d &obj, int value);
    friend three_d operator+(int value, const three_d &obj);
    
    // Допоміжні методи
    int get_x() const { return x; }
    int get_y() const { return y; }
    int get_z() const { return z; }
    
    void set(int i, int j, int k) { x = i; y = j; z = k; }
    
    bool is_zero() const {
        return x == 0 && y == 0 && z == 0;
    }
    
    three_d& operator=(const three_d &obj) {
        x = obj.x;
        y = obj.y;
        z = obj.z;
        return *this;
    }
};

// Реалізація дружніх функцій

// Префіксний інкремент
three_d& operator++(three_d &obj) {
    ++obj.x;
    ++obj.y;
    ++obj.z;
    return obj;
}

// Префіксний декремент
three_d& operator--(three_d &obj) {
    --obj.x;
    --obj.y;
    --obj.z;
    return obj;
}

// obj + int
three_d operator+(const three_d &obj, int value) {
    three_d temp;
    temp.x = obj.x + value;
    temp.y = obj.y + value;
    temp.z = obj.z + value;
    return temp;
}

// int + obj
three_d operator+(int value, const three_d &obj) {
    three_d temp;
    temp.x = value + obj.x;
    temp.y = value + obj.y;
    temp.z = value + obj.z;
    return temp;
}

int main() {
    cout << "ДЕМОНСТРАЦІЯ ПАРАМЕТРІВ-ПОСИЛАНЬ ТА ДРУЖНІХ ФУНКЦІЙ:\n";
    cout << "===================================================\n\n";
    
    // Створення об'єктів
    cout << "1. СТВОРЕННЯ ОБ'ЄКТІВ three_d:\n";
    cout << "==============================\n";
    
    three_d point1(5, 10, 15);
    three_d point2(3, 7, 12);
    three_d point3(0, 0, 0);
    three_d result;
    
    cout << "point1: "; point1.show(); cout << "\n";
    cout << "point2: "; point2.show(); cout << "\n";
    cout << "point3: "; point3.show(); cout << " (нульовий)\n";
    
    // Оператори з параметрами-посиланнями
    cout << "\n2. ОПЕРАТОРИ З ПАРАМЕТРАМИ-ПОСИЛАННЯМИ:\n";
    cout << "======================================\n";
    
    cout << "point1 + point2 = ";
    three_d &sum_ref = point1 + point2;
    sum_ref.show();
    cout << " (повернуто посилання)\n";
    
    cout << "point1 - point2 = ";
    three_d &diff_ref = point1 - point2;
    diff_ref.show();
    cout << " (повернуто посилання)\n";
    
    // Оператори порівняння
    cout << "\n3. ОПЕРАТОРИ ПОРІВНЯННЯ:\n";
    cout << "=======================\n";
    
    three_d equal1(5, 5, 5);
    three_d equal2(5, 5, 5);
    three_d different(1, 2, 3);
    
    cout << "equal1: "; equal1.show(); cout << "\n";
    cout << "equal2: "; equal2.show(); cout << "\n";
    cout << "different: "; different.show(); cout << "\n";
    
    cout << "\nРезультати порівняння:\n";
    cout << "equal1 == equal2: " << (equal1 == equal2 ? "true" : "false") << "\n";
    cout << "equal1 == different: " << (equal1 == different ? "true" : "false") << "\n";
    cout << "equal1 != different: " << (equal1 != different ? "true" : "false") << "\n";
    
    // Логічне АБО
    cout << "\n4. ОПЕРАТОР ЛОГІЧНОГО АБО (||):\n";
    cout << "==============================\n";
    
    three_d zero_point(0, 0, 0);
    three_d non_zero(1, 0, 0);
    
    cout << "zero_point: "; zero_point.show(); cout << "\n";
    cout << "non_zero: "; non_zero.show(); cout << "\n";
    cout << "point1: "; point1.show(); cout << "\n";
    
    cout << "\nРезультати логічного АБО:\n";
    cout << "zero_point || non_zero: " << (zero_point || non_zero ? "true" : "false") << "\n";
    cout << "zero_point || zero_point: " << (zero_point || zero_point ? "true" : "false") << "\n";
    cout << "point1 || zero_point: " << (point1 || zero_point ? "true" : "false") << "\n";
    
    // Дружні функції для інкремента/декремента
    cout << "\n5. ДРУЖНІ ФУНКЦІЇ ІНКРЕМЕНТА/ДЕКРЕМЕНТА:\n";
    cout << "=======================================\n";
    
    three_d inc_test(10, 20, 30);
    cout << "inc_test до інкремента: "; inc_test.show(); cout << "\n";
    
    three_d &inc_result = ++inc_test;
    cout << "result = ++inc_test: "; inc_result.show(); cout << "\n";
    cout << "inc_test після інкремента: "; inc_test.show(); cout << "\n";
    cout << "Адреси однакові: " << (&inc_result == &inc_test ? "true" : "false") << "\n";
    
    three_d dec_test(10, 20, 30);
    cout << "\ndec_test до декремента: "; dec_test.show(); cout << "\n";
    
    three_d &dec_result = --dec_test;
    cout << "result = --dec_test: "; dec_result.show(); cout << "\n";
    cout << "dec_test після декремента: "; dec_test.show(); cout << "\n";
    cout << "Адреси однакові: " << (&dec_result == &dec_test ? "true" : "false") << "\n";
    
    // Змішані операції obj+int та int+obj
    cout << "\n6. ЗМІШАНІ ОПЕРАЦІЇ (obj+int та int+obj):\n";
    cout << "========================================\n";
    
    three_d base_point(10, 20, 30);
    cout << "base_point: "; base_point.show(); cout << "\n";
    
    cout << "\nОперації obj + int:\n";
    result = base_point + 5;
    cout << "base_point + 5 = "; result.show(); cout << "\n";
    
    result = base_point + (-3);
    cout << "base_point + (-3) = "; result.show(); cout << "\n";
    
    cout << "\nОперації int + obj:\n";
    result = 7 + base_point;
    cout << "7 + base_point = "; result.show(); cout << "\n";
    
    result = (-2) + base_point;
    cout << "(-2) + base_point = "; result.show(); cout << "\n";
    
    // Перевірка комутативності
    cout << "\n7. ПЕРЕВІРКА КОМУТАТИВНОСТІ:\n";
    cout << "============================\n";
    
    three_d test_point(1, 2, 3);
    int add_value = 10;
    
    three_d result1 = test_point + add_value;
    three_d result2 = add_value + test_point;
    
    cout << "test_point: "; test_point.show(); cout << "\n";
    cout << "test_point + " << add_value << " = "; result1.show(); cout << "\n";
    cout << add_value << " + test_point = "; result2.show(); cout << "\n";
    cout << "Результати однакові: " << (result1 == result2 ? "true" : "false") << "\n";
    
    // Комплексні вирази
    cout << "\n8. КОМПЛЕКСНІ ВИРАЗИ:\n";
    cout << "====================\n";
    
    three_d a(1, 2, 3);
    three_d b(4, 5, 6);
    
    cout << "a: "; a.show(); cout << "\n";
    cout << "b: "; b.show(); cout << "\n";
    
    // Комбінація різних операцій
    result = (a + 10) + (5 + b);
    cout << "\n(a + 10) + (5 + b) = "; result.show(); cout << "\n";
    cout << "Пояснення: (1+10, 2+10, 3+10) + (5+4, 5+5, 5+6) = (11, 12, 13) + (9, 10, 11) = (20, 22, 24)\n";
    
    // Використання з інкрементом
    three_d temp_a = a;
    three_d temp_b = b;
    result = (++temp_a) + (temp_b + 1);
    cout << "\n(++a_copy) + (b_copy + 1) = "; result.show(); cout << "\n";
    
    // Практичні застосування
    cout << "\n9. ПРАКТИЧНІ ЗАСТОСУВАННЯ:\n";
    cout << "=========================\n";
    
    // Трансляція всіх точок на фіксовану відстань
    three_d points[] = {
        three_d(0, 0, 0),
        three_d(10, 10, 10),
        three_d(-5, 5, -5),
        three_d(3, 7, 2)
    };
    int num_points = 4;
    int translation = 5;
    
    cout << "Оригінальні точки:\n";
    for(int i = 0; i < num_points; i++) {
        cout << "Point " << i << ": "; points[i].show(); cout << "\n";
    }
    
    cout << "\nПісля трансляції на " << translation << ":\n";
    for(int i = 0; i < num_points; i++) {
        three_d translated = points[i] + translation;
        cout << "Point " << i << ": "; translated.show(); cout << "\n";
    }
    
    // Симуляція зростання
    cout << "\n10. СИМУЛЯЦІЯ ЗРОСТАННЯ:\n";
    cout << "=======================\n";
    
    three_d organism(1, 1, 1);
    cout << "Початковий розмір організму: "; organism.show(); cout << "\n";
    
    cout << "Процес зростання (інкремент кожну ітерацію):\n";
    for(int generation = 1; generation <= 5; generation++) {
        ++organism;
        cout << "Покоління " << generation << ": "; organism.show(); cout << "\n";
    }
    
    // Порівняння з нульовими точками
    cout << "\n11. РОБОТА З НУЛЬОВИМИ ТОЧКАМИ:\n";
    cout << "==============================\n";
    
    three_d zero1(0, 0, 0);
    three_d zero2(0, 0, 0);
    three_d non_zero(1, 2, 3);
    
    cout << "zero1: "; zero1.show(); cout << " is_zero: " << zero1.is_zero() << "\n";
    cout << "zero2: "; zero2.show(); cout << " is_zero: " << zero2.is_zero() << "\n";
    cout << "non_zero: "; non_zero.show(); cout << " is_zero: " << non_zero.is_zero() << "\n";
    
    cout << "\nЛогічні операції:\n";
    cout << "zero1 == zero2: " << (zero1 == zero2) << "\n";
    cout << "zero1 || zero2: " << (zero1 || zero2) << "\n";
    cout << "zero1 || non_zero: " << (zero1 || non_zero) << "\n";
    
    // Ланцюгові операції
    cout << "\n12. ЛАНЦЮГОВІ ОПЕРАЦІЇ:\n";
    cout << "======================\n";
    
    three_d chain_test(5, 5, 5);
    cout << "chain_test: "; chain_test.show(); cout << "\n";
    
    // Ланцюгові додавання
    result = chain_test + 2 + 3;  // (chain_test + 2) + 3
    cout << "chain_test + 2 + 3 = "; result.show(); cout << "\n";
    
    result = 1 + chain_test + 4;  // (1 + chain_test) + 4
    cout << "1 + chain_test + 4 = "; result.show(); cout << "\n";
    
    cout << "\n13. ПЕРЕВАГИ ВИКОРИСТАННЯ ПОСИЛАНЬ:\n";
    cout << "===================================\n";
    cout << "+ Ефективність: уникнення копіювання об'єктів\n";
    cout << "+ Швидкість: передача посилань швидша за передачу за значенням\n";
    cout << "+ Пам'ять: економія пам'яті, особливо для великих об'єктів\n";
    cout << "+ Гнучкість: можливість модифікації оригінальних об'єктів\n";
    
    cout << "\nПЕРЕВАГИ ДРУЖНІХ ФУНКЦІЙ:\n";
    cout << "+ Природний синтаксис для змішаних операцій\n";
    cout << "+ Симетричність операцій (int+obj та obj+int)\n";
    cout << "+ Доступ до приватних членів\n";
    cout << "+ Можливість оптимізації реалізації\n";
    
    return 0;
}