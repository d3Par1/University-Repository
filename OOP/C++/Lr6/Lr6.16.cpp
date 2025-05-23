#include <iostream>
using namespace std;

class three_d {
    int x, y, z;
public:
    three_d(int i, int j, int k) { x = i; y = j; z = k; }
    three_d() { x = 0; y = 0; z = 0; }
    void get(int &i, int &j, int &k) { i = x; j = y; k = z; }
    void show() { cout << "(" << x << ", " << y << ", " << z << ")"; }
    
    // Перевантаження оператора додавання
    three_d operator+(const three_d &obj) {
        three_d temp;
        temp.x = x + obj.x;
        temp.y = y + obj.y;
        temp.z = z + obj.z;
        return temp;
    }
    
    // Перевантаження оператора віднімання
    three_d operator-(const three_d &obj) {
        three_d temp;
        temp.x = x - obj.x;
        temp.y = y - obj.y;
        temp.z = z - obj.z;
        return temp;
    }
    
    // Префіксний інкремент (тільки префіксна форма)
    three_d& operator++() {
        ++x;
        ++y;
        ++z;
        return *this;
    }
    
    // Префіксний декремент (тільки префіксна форма)
    three_d& operator--() {
        --x;
        --y;
        --z;
        return *this;
    }
    
    // Допоміжні оператори
    three_d& operator=(const three_d &obj) {
        x = obj.x;
        y = obj.y;
        z = obj.z;
        return *this;
    }
    
    bool operator==(const three_d &obj) {
        return (x == obj.x) && (y == obj.y) && (z == obj.z);
    }
    
    bool operator!=(const three_d &obj) {
        return !(*this == obj);
    }
    
    // Додаткові корисні методи
    int get_x() const { return x; }
    int get_y() const { return y; }
    int get_z() const { return z; }
    
    void set(int i, int j, int k) {
        x = i; y = j; z = k;
    }
    
    // Обчислення відстані від початку координат
    double distance_from_origin() const {
        return sqrt(x*x + y*y + z*z);
    }
    
    // Обчислення відстані до іншої точки
    double distance_to(const three_d &other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        int dz = z - other.z;
        return sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    // Скалярний добуток
    int dot_product(const three_d &other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    // Векторний добуток
    three_d cross_product(const three_d &other) const {
        three_d result;
        result.x = y * other.z - z * other.y;
        result.y = z * other.x - x * other.z;
        result.z = x * other.y - y * other.x;
        return result;
    }
};

int main() {
    cout << "ДЕМОНСТРАЦІЯ ОПЕРАТОРІВ ДЛЯ КЛАСУ three_d:\n";
    cout << "==========================================\n\n";
    
    // Створення об'єктів
    cout << "1. СТВОРЕННЯ 3D ТОЧОК:\n";
    cout << "=====================\n";
    
    three_d point1(5, 10, 15);
    three_d point2(3, 7, 12);
    three_d point3(-2, 4, -8);
    three_d result;
    
    cout << "point1: "; point1.show(); cout << "\n";
    cout << "point2: "; point2.show(); cout << "\n";
    cout << "point3: "; point3.show(); cout << "\n";
    
    // Демонстрація оператора додавання
    cout << "\n2. ОПЕРАТОР ДОДАВАННЯ (+):\n";
    cout << "=========================\n";
    
    result = point1 + point2;
    cout << "point1 + point2 = "; result.show(); cout << "\n";
    cout << "Перевірка: (5+3, 10+7, 15+12) = (8, 17, 27)\n";
    
    result = point1 + point3;
    cout << "point1 + point3 = "; result.show(); cout << "\n";
    cout << "Перевірка: (5+(-2), 10+4, 15+(-8)) = (3, 14, 7)\n";
    
    result = point2 + point3;
    cout << "point2 + point3 = "; result.show(); cout << "\n";
    cout << "Перевірка: (3+(-2), 7+4, 12+(-8)) = (1, 11, 4)\n";
    
    // Демонстрація оператора віднімання
    cout << "\n3. ОПЕРАТОР ВІДНІМАННЯ (-):\n";
    cout << "==========================\n";
    
    result = point1 - point2;
    cout << "point1 - point2 = "; result.show(); cout << "\n";
    cout << "Перевірка: (5-3, 10-7, 15-12) = (2, 3, 3)\n";
    
    result = point2 - point1;
    cout << "point2 - point1 = "; result.show(); cout << "\n";
    cout << "Перевірка: (3-5, 7-10, 12-15) = (-2, -3, -3)\n";
    
    result = point1 - point3;
    cout << "point1 - point3 = "; result.show(); cout << "\n";
    cout << "Перевірка: (5-(-2), 10-4, 15-(-8)) = (7, 6, 23)\n";
    
    // Демонстрація префіксного інкремента
    cout << "\n4. ПРЕФІКСНИЙ ІНКРЕМЕНТ (++obj):\n";
    cout << "===============================\n";
    
    three_d inc_test(10, 20, 30);
    cout << "inc_test до інкремента: "; inc_test.show(); cout << "\n";
    
    three_d inc_result = ++inc_test;
    cout << "result = ++inc_test: "; inc_result.show(); cout << "\n";
    cout << "inc_test після інкремента: "; inc_test.show(); cout << "\n";
    cout << "Зверніть увагу: обидва об'єкти однакові (префіксна форма)\n";
    
    // Ланцюгові інкременти
    cout << "\nЛанцюгові інкременти:\n";
    cout << "inc_test до: "; inc_test.show(); cout << "\n";
    ++(++inc_test);  // Подвійний префіксний інкремент
    cout << "Після ++(++inc_test): "; inc_test.show(); cout << "\n";
    
    // Демонстрація префіксного декремента
    cout << "\n5. ПРЕФІКСНИЙ ДЕКРЕМЕНТ (--obj):\n";
    cout << "===============================\n";
    
    three_d dec_test(10, 20, 30);
    cout << "dec_test до декремента: "; dec_test.show(); cout << "\n";
    
    three_d dec_result = --dec_test;
    cout << "result = --dec_test: "; dec_result.show(); cout << "\n";
    cout << "dec_test після декремента: "; dec_test.show(); cout << "\n";
    cout << "Обидва об'єкти однакові (префіксна форма)\n";
    
    // Комбіновані операції
    cout << "\n6. КОМБІНОВАНІ ОПЕРАЦІЇ:\n";
    cout << "=======================\n";
    
    three_d a(1, 2, 3);
    three_d b(4, 5, 6);
    three_d c(7, 8, 9);
    
    cout << "a: "; a.show(); cout << "\n";
    cout << "b: "; b.show(); cout << "\n";
    cout << "c: "; c.show(); cout << "\n";
    
    // Складний вираз
    result = (a + b) - c;
    cout << "\n(a + b) - c = "; result.show(); cout << "\n";
    cout << "Перевірка: ((1+4, 2+5, 3+6) - (7, 8, 9)) = (5, 7, 9) - (7, 8, 9) = (-2, -1, 0)\n";
    
    result = a + (b - c);
    cout << "a + (b - c) = "; result.show(); cout << "\n";
    cout << "Перевірка: (1, 2, 3) + ((4-7, 5-8, 6-9)) = (1, 2, 3) + (-3, -3, -3) = (-2, -1, 0)\n";
    
    // Операції з інкрементом
    three_d temp_a = a;
    three_d temp_b = b;
    result = (++temp_a) + (++temp_b);
    cout << "\n(++a_copy) + (++b_copy) = "; result.show(); cout << "\n";
    
    // Використання в циклах
    cout << "\n7. ВИКОРИСТАННЯ В ЦИКЛАХ:\n";
    cout << "========================\n";
    
    three_d counter(0, 0, 0);
    cout << "Лічильник в циклі (до 3):\n";
    
    for(int i = 0; i < 3; i++) {
        cout << "Ітерація " << i << ": "; counter.show(); cout << "\n";
        ++counter;
    }
    cout << "Фінальне значення: "; counter.show(); cout << "\n";
    
    // Обернений відлік
    cout << "\nОбернений відлік:\n";
    three_d countdown(3, 3, 3);
    while(countdown.get_x() > 0) {
        cout << "Відлік: "; countdown.show(); cout << "\n";
        --countdown;
    }
    cout << "Кінець відліку: "; countdown.show(); cout << "\n";
    
    // Практичні застосування
    cout << "\n8. ПРАКТИЧНІ ЗАСТОСУВАННЯ:\n";
    cout << "=========================\n";
    
    // Переміщення в 3D просторі
    three_d position(10, 20, 30);
    three_d velocity(2, -1, 3);
    
    cout << "Симуляція руху в 3D просторі:\n";
    cout << "Початкова позиція: "; position.show(); cout << "\n";
    cout << "Швидкість: "; velocity.show(); cout << "\n";
    
    for(int time = 1; time <= 3; time++) {
        position = position + velocity;
        cout << "Час " << time << ": позиція "; position.show(); cout << "\n";
    }
    
    // Центр мас
    cout << "\nОбчислення центру мас:\n";
    three_d object1(0, 0, 0);
    three_d object2(10, 0, 0);
    three_d object3(5, 10, 0);
    
    cout << "Об'єкт 1: "; object1.show(); cout << "\n";
    cout << "Об'єкт 2: "; object2.show(); cout << "\n";
    cout << "Об'єкт 3: "; object3.show(); cout << "\n";
    
    three_d center_of_mass = (object1 + object2 + object3);
    // Ділимо координати на 3 (кількість об'єктів)
    int x, y, z;
    center_of_mass.get(x, y, z);
    center_of_mass.set(x/3, y/3, z/3);
    
    cout << "Центр мас: "; center_of_mass.show(); cout << "\n";
    
    // Векторні операції
    cout << "\n9. ВЕКТОРНІ ОПЕРАЦІЇ:\n";
    cout << "===================\n";
    
    three_d vector1(3, 4, 5);
    three_d vector2(1, 2, 3);
    
    cout << "Вектор 1: "; vector1.show(); cout << "\n";
    cout << "Вектор 2: "; vector2.show(); cout << "\n";
    
    // Сума векторів
    three_d vector_sum = vector1 + vector2;
    cout << "Сума векторів: "; vector_sum.show(); cout << "\n";
    
    // Різниця векторів
    three_d vector_diff = vector1 - vector2;
    cout << "Різниця векторів: "; vector_diff.show(); cout << "\n";
    
    // Скалярний добуток
    int dot = vector1.dot_product(vector2);
    cout << "Скалярний добуток: " << dot << "\n";
    
    // Векторний добуток
    three_d cross = vector1.cross_product(vector2);
    cout << "Векторний добуток: "; cross.show(); cout << "\n";
    
    // Порівняння об'єктів
    cout << "\n10. ПОРІВНЯННЯ ОБ'ЄКТІВ:\n";
    cout << "=======================\n";
    
    three_d p1(5, 5, 5);
    three_d p2(5, 5, 5);
    three_d p3(3, 4, 5);
    
    cout << "p1: "; p1.show(); cout << "\n";
    cout << "p2: "; p2.show(); cout << "\n";
    cout << "p3: "; p3.show(); cout << "\n";
    
    cout << "\nРезультати порівняння:\n";
    cout << "p1 == p2: " << (p1 == p2 ? "true" : "false") << "\n";
    cout << "p1 == p3: " << (p1 == p3 ? "true" : "false") << "\n";
    cout << "p1 != p3: " << (p1 != p3 ? "true" : "false") << "\n";
    
    cout << "\n11. ОСОБЛИВОСТІ РЕАЛІЗАЦІЇ:\n";
    cout << "===========================\n";
    cout << "• Реалізовано тільки префіксні форми ++ та --\n";
    cout << "• Кожна операція змінює всі три координати одночасно\n";
    cout << "• Оператори повертають нові об'єкти (додавання, віднімання)\n";
    cout << "• Інкремент/декремент повертають посилання на поточний об'єкт\n";
    cout << "• Всі операції зберігають цілочисельний тип координат\n";
    
    return 0;
}