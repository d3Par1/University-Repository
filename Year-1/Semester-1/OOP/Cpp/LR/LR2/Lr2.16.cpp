#include <iostream>

using namespace std;

class stack {
private:
    int* stck; // Вказівник на стек
    int tos;   // Індекс вершини стеку
    int size;  // Розмір стеку
    
public:
    // Конструктор з параметром розміру стеку
    stack(int stackSize) {
        size = stackSize;
        stck = new int[size];
        
        if (!stck) {
            cout << "Помилка виділення пам'яті\n";
            exit(1);
        }
        
        tos = 0; // ініціалізація індексу вершини стеку
        cout << "Стек створено з розміром " << size << endl;
    }
    
    // Деструктор для звільнення пам'яті
    ~stack() {
        cout << "Звільнення пам'яті стеку\n";
        delete[] stck;
    }
    
    // Додавання елементу до стеку
    void push(int item) {
        if (tos == size) {
            cout << "Стек повний\n";
            return;
        }
        
        stck[tos] = item;
        tos++;
    }
    
    // Видалення елементу зі стеку
    int pop() {
        if (tos == 0) {
            cout << "Стек порожній\n";
            return 0; // повертаємо 0 в якості індикатора помилки
        }
        
        tos--;
        return stck[tos];
    }
    
    // Перевірка, чи стек порожній
    bool isEmpty() {
        return tos == 0;
    }
    
    // Перевірка, чи стек повний
    bool isFull() {
        return tos == size;
    }
    
    // Показати поточний розмір стеку
    int currentSize() {
        return tos;
    }
};

int main() {
    // Створюємо стек розміром 5 елементів
    stack s(5);
    
    cout << "Додаємо елементи до стеку:\n";
    for (int i = 1; i <= 6; i++) {
        cout << "Додаємо " << i * 10 << ": ";
        s.push(i * 10);
    }
    
    cout << "\nПоточний розмір стеку: " << s.currentSize() << endl;
    cout << "Стек порожній: " << (s.isEmpty() ? "так" : "ні") << endl;
    cout << "Стек повний: " << (s.isFull() ? "так" : "ні") << endl;
    
    cout << "\nВидаляємо елементи зі стеку:\n";
    while (!s.isEmpty()) {
        cout << "Видалено: " << s.pop() << endl;
    }
    
    cout << "\nСпробуємо видалити ще один елемент:\n";
    s.pop();
    
    // Створюємо ще один стек з іншим розміром
    cout << "\nСтворюємо ще один стек розміром 3:\n";
    stack s2(3);
    
    cout << "Додаємо елементи до нового стеку:\n";
    s2.push(100);
    s2.push(200);
    s2.push(300);
    
    cout << "\nВидаляємо елементи з нового стеку:\n";
    while (!s2.isEmpty()) {
        cout << "Видалено: " << s2.pop() << endl;
    }
    
    return 0;
}