#include <iostream>
using namespace std;

// Клас для представлення часу (години, хвилини)
class Time {
    int hours;
    int minutes;
    
public:
    Time() { hours = 0; minutes = 0; }
    Time(int h, int m) {
        hours = h;
        minutes = m;
        normalize();  // нормалізуємо час
    }
    
    void show() {
        cout << (hours < 10 ? "0" : "") << hours << ":" 
             << (minutes < 10 ? "0" : "") << minutes;
    }
    
    // Нормалізація часу (перенесення хвилин в години)
    void normalize() {
        if (minutes >= 60) {
            hours += minutes / 60;
            minutes = minutes % 60;
        }
        if (minutes < 0) {
            hours -= (-minutes - 1) / 60 + 1;
            minutes = 60 - ((-minutes - 1) % 60 + 1);
        }
        if (hours >= 24) {
            hours = hours % 24;
        }
        if (hours < 0) {
            hours = 24 + (hours % 24);
        }
    }
    
    // Префіксний інкремент (++obj) - додає 1 хвилину
    Time& operator++() {
        minutes++;
        normalize();
        return *this;
    }
    
    // Постфіксний інкремент (obj++) - додає 1 хвилину
    Time operator++(int) {
        Time temp = *this;
        minutes++;
        normalize();
        return temp;
    }
    
    // Префіксний декремент (--obj) - віднімає 1 хвилину
    Time& operator--() {
        minutes--;
        normalize();
        return *this;
    }
    
    // Постфіксний декремент (obj--) - віднімає 1 хвилину  
    Time operator--(int) {
        Time temp = *this;
        minutes--;
        normalize();
        return temp;
    }
    
    // Оператор присвоєння
    Time& operator=(const Time &other) {
        hours = other.hours;
        minutes = other.minutes;
        return *this;
    }
    
    // Оператор порівняння
    bool operator==(const Time &other) {
        return hours == other.hours && minutes == other.minutes;
    }
    
    bool operator!=(const Time &other) {
        return !(*this == other);
    }
};

// Дружні функції для демонстрації альтернативного підходу
class Counter {
    int value;
    
public:
    Counter(int v = 0) : value(v) {}
    
    void show() { cout << "Counter: " << value; }
    int get_value() const { return value; }
    
    // Дружні функції для операторів інкремента/декремента
    friend Counter& operator++(Counter &obj);              // префіксний ++
    friend Counter operator++(Counter &obj, int);          // постфіксний ++
    friend Counter& operator--(Counter &obj);              // префіксний --
    friend Counter operator--(Counter &obj, int);          // постфіксний --
    
    Counter& operator=(const Counter &other) {
        value = other.value;
        return *this;
    }
    
    bool operator==(const Counter &other) {
        return value == other.value;
    }
};

// Реалізація дружніх функцій
Counter& operator++(Counter &obj) {
    ++obj.value;
    return obj;
}

Counter operator++(Counter &obj, int) {
    Counter temp = obj;
    ++obj.value;
    return temp;
}

Counter& operator--(Counter &obj) {
    --obj.value;
    return obj;
}

Counter operator--(Counter &obj, int) {
    Counter temp = obj;
    --obj.value;
    return temp;
}

int main() {
    cout << "ДЕМОНСТРАЦІЯ ПРЕФІКСНИХ ТА ПОСТФІКСНИХ ОПЕРАТОРІВ:\n";
    cout << "==================================================\n\n";
    
    // Демонстрація з класом Time
    cout << "1. КЛАС TIME (ГОДИНИ:ХВИЛИНИ):\n";
    cout << "==============================\n\n";
    
    Time time1(10, 58);
    Time time2(10, 58);
    Time result;
    
    cout << "Початковий час: ";
    time1.show();
    cout << "\n\n";
    
    // Префіксний інкремент
    cout << "Префіксний інкремент (++time):\n";
    cout << "time до операції: "; time1.show(); cout << "\n";
    result = ++time1;
    cout << "result = ++time: "; result.show(); cout << "\n";
    cout << "time після операції: "; time1.show(); cout << "\n";
    cout << "Зверніть увагу: хвилини перейшли в години!\n\n";
    
    // Постфіксний інкремент
    cout << "Постфіксний інкремент (time++):\n";
    cout << "time до операції: "; time2.show(); cout << "\n";
    result = time2++;
    cout << "result = time++: "; result.show(); cout << "\n";
    cout << "time після операції: "; time2.show(); cout << "\n";
    cout << "result містить старе значення!\n\n";
    
    // Демонстрація переходу через межі
    cout << "Демонстрація переходу через межі:\n";
    Time time3(23, 59);
    cout << "Час 23:59, застосовуємо ++:\n";
    cout << "До: "; time3.show(); cout << "\n";
    ++time3;
    cout << "Після ++: "; time3.show(); cout << " (перехід на наступний день)\n\n";
    
    // Декремент через межі
    Time time4(0, 0);
    cout << "Час 00:00, застосовуємо --:\n";
    cout << "До: "; time4.show(); cout << "\n";
    --time4;
    cout << "Після --: "; time4.show(); cout << " (перехід на попередній день)\n\n";
    
    // Демонстрація з класом Counter (дружні функції)
    cout << "2. КЛАС COUNTER (ДРУЖНІ ФУНКЦІЇ):\n";
    cout << "=================================\n\n";
    
    Counter counter1(5);
    Counter counter2(5);
    Counter counter_result;
    
    cout << "Початкове значення: ";
    counter1.show();
    cout << "\n\n";
    
    // Префіксний інкремент з дружньою функцією
    cout << "Префіксний інкремент (дружня функція):\n";
    cout << "counter до операції: "; counter1.show(); cout << "\n";
    counter_result = ++counter1;
    cout << "result = ++counter: "; counter_result.show(); cout << "\n";
    cout << "counter після операції: "; counter1.show(); cout << "\n";
    cout << "Значення однакові (префіксна форма)\n\n";
    
    // Постфіксний інкремент з дружньою функцією
    cout << "Постфіксний інкремент (дружня функція):\n";
    cout << "counter до операції: "; counter2.show(); cout << "\n";
    counter_result = counter2++;
    cout << "result = counter++: "; counter_result.show(); cout << "\n";
    cout << "counter після операції: "; counter2.show(); cout << "\n";
    cout << "result містить старе значення!\n\n";
    
    // Практичне використання в циклах
    cout << "3. ПРАКТИЧНЕ ВИКОРИСТАННЯ В ЦИКЛАХ:\n";
    cout << "===================================\n\n";
    
    cout << "Цикл з префіксним декрементом:\n";
    Counter loop_counter1(3);
    while(loop_counter1.get_value() > 0) {
        cout << "Значення: "; loop_counter1.show(); cout << "\n";
        --loop_counter1;  // спочатку зменшує, потім використовує
    }
    
    cout << "\nЦикл з постфіксним декрементом:\n";
    Counter loop_counter2(3);
    while(loop_counter2.get_value() > 0) {
        cout << "Значення: "; loop_counter2.show(); cout << "\n";
        loop_counter2--;  // спочатку використовує, потім зменшує
    }
    
    // Демонстрація ефективності
    cout << "\n4. ПОРІВНЯННЯ ЕФЕКТИВНОСТІ:\n";
    cout << "===========================\n\n";
    
    cout << "Префіксна форма (++obj):\n";
    cout << "- Більш ефективна\n";
    cout << "- Не створює тимчасових об'єктів\n";
    cout << "- Повертає посилання на об'єкт\n";
    cout << "- Рекомендується використовувати, коли не потрібно старе значення\n\n";
    
    cout << "Постфіксна форма (obj++):\n";
    cout << "- Менш ефективна\n";
    cout << "- Створює тимчасовий об'єкт для зберігання старого значення\n";
    cout << "- Повертає копію старого значення\n";
    cout << "- Використовувати тільки коли потрібно старе значення\n\n";
    
    // Комплексний приклад
    cout << "5. КОМПЛЕКСНИЙ ПРИКЛАД:\n";
    cout << "======================\n\n";
    
    Time alarm(7, 59);
    cout << "Будильник встановлено на: "; alarm.show(); cout << "\n";
    
    cout << "Відлік останніх секунд:\n";
    for(int i = 0; i < 3; i++) {
        cout << "Поточний час: "; alarm.show(); cout << "\n";
        Time temp = alarm++;  // зберігаємо поточний час, потім збільшуємо
        cout << "Збережений час: "; temp.show(); cout << "\n";
        cout << "---\n";
    }
    
    cout << "Будильник спрацював о: "; alarm.show(); cout << "\n";
    
    return 0;
}