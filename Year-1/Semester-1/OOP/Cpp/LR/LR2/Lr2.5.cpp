#include <iostream>

using namespace std;

#define SIZE 100

class q_type {
    int queue[SIZE]; // містить чергу
    int head, tail;  // індекс вершини і хвоста
public:
    q_type();        // конструктор
    void q(int num); // запам'ятовування
    int deq();       // відновлення
    bool isEmpty();  // перевірка, чи черга порожня
    bool isFull();   // перевірка, чи черга повна
};

// Конструктор - ініціалізує чергу
q_type::q_type() {
    head = 0;
    tail = 0;
}

// Запам'ятовування елемента в черзі
void q_type::q(int num) {
    if (isFull()) {
        cout << "Черга повна!" << endl;
        return;
    }
    
    queue[tail] = num;
    tail = (tail + 1) % SIZE;
}

// Видалення і повернення елемента з черги
int q_type::deq() {
    if (isEmpty()) {
        cout << "Черга порожня!" << endl;
        return -1;
    }
    
    int val = queue[head];
    head = (head + 1) % SIZE;
    return val;
}

// Перевірка, чи черга порожня
bool q_type::isEmpty() {
    return head == tail;
}

// Перевірка, чи черга повна
bool q_type::isFull() {
    return (tail + 1) % SIZE == head;
}

int main() {
    q_type intQueue;
    int value;
    
    cout << "Демонстрація роботи циклічної черги:" << endl;
    
    // Додаємо елементи в чергу
    cout << "\nДодаємо елементи в чергу:" << endl;
    for (int i = 1; i <= 5; i++) {
        cout << "Додаємо " << i * 10 << endl;
        intQueue.q(i * 10);
    }
    
    // Отримуємо елементи з черги
    cout << "\nОтримуємо елементи з черги:" << endl;
    for (int i = 1; i <= 3; i++) {
        value = intQueue.deq();
        if (value != -1)
            cout << "Отримано " << value << endl;
    }
    
    // Додаємо ще елементи
    cout << "\nДодаємо ще елементи:" << endl;
    for (int i = 6; i <= 8; i++) {
        cout << "Додаємо " << i * 10 << endl;
        intQueue.q(i * 10);
    }
    
    // Отримуємо решту елементів
    cout << "\nОтримуємо решту елементів:" << endl;
    while (!intQueue.isEmpty()) {
        value = intQueue.deq();
        if (value != -1)
            cout << "Отримано " << value << endl;
    }
    
    return 0;
}