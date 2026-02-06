#include <iostream>

class CountDown {
    int incr; 
    int target;
    mutable int current;  // Позначаємо current як mutable, щоб можна було змінювати в const функції
public:
    CountDown(int delay, int i=1) { 
        target = delay;
        incr = i; 
        current = 0;
    }
    
    // Помилка була тут: функція позначена як const, але намагається модифікувати current
    bool counting() const { 
        current += incr;  // Тепер це дозволено, оскільки current є mutable
        if (current >= target) { 
            std::cout << "\a";  // Звуковий сигнал
            return false;
        }
        std::cout << current << " "; 
        return true;
    }
};

int main() {
    CountDown obj(100, 2); 
    while (obj.counting());  // Викликає counting() доки не повернеться false
    return 0;
}

// Пояснення помилки:
// Проблема була в тому, що метод counting() позначено як const,
// але він намагається змінити змінну-член current.
// Є два способи виправити цю помилку:
// 1. Видалити const з оголошення методу counting()
// 2. Зробити змінну current mutable, як у цьому рішенні
//
// Для reference, ось оригінальний код з помилкою:
/*
class CountDown {
    int incr; 
    int target;
    int current;
public:
    CountDown(int delay, int i=1) { 
        target = delay;
        incr = i; 
        current = 0;
    }
    
    bool counting() const {   // const метод не може змінювати змінні-члени
        current += incr;      // Помилка: модифікація змінної в const функції
        if (current >= target) { 
            cout << "\a"; 
            return false;
        }
        cout << current << " "; 
        return true;
    }
};
*/