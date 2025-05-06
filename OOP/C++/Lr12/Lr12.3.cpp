#include <iostream>
using namespace std;

template <class StackType>
class Stack {
    static const int SIZE = 100;
    StackType stck[SIZE];
    int tos;
public:
    void init();
    void push(StackType ch);
    StackType pop();
};

template <class StackType>
void Stack<StackType>::init() {
    tos = 0;
}

template <class StackType>
void Stack<StackType>::push(StackType ch) {
    if(tos == SIZE) {
        cout << "Стек переповнений.\n";
        return;
    }
    stck[tos] = ch;
    tos++;
}

template <class StackType>
StackType Stack<StackType>::pop() {
    if(tos == 0) {
        cout << "Стек порожній.\n";
        return StackType();
    }
    tos--;
    return stck[tos];
}

template <class QueueType>
class Queue {
    static const int SIZE = 100;
    QueueType queue[SIZE]; 
    int head, tail;
    int count; 
public:
    void init();
    void enqueue(QueueType obj);
    QueueType dequeue();
    bool isEmpty();
    bool isFull();
};

template <class QueueType>
void Queue<QueueType>::init() {
    head = 0;
    tail = 0;
    count = 0;
}

template <class QueueType>
void Queue<QueueType>::enqueue(QueueType obj) {
    if(isFull()) {
        cout << "Черга заповнена.\n";
        return;
    }
    queue[tail] = obj;
    tail = (tail + 1) % SIZE;
    count++;
}

template <class QueueType>
QueueType Queue<QueueType>::dequeue() {
    if(isEmpty()) {
        cout << "Черга порожня.\n";
        return QueueType();
    }
    QueueType obj = queue[head];
    head = (head + 1) % SIZE; 
    count--;
    return obj;
}

template <class QueueType>
bool Queue<QueueType>::isEmpty() {
    return count == 0;
}

template <class QueueType>
bool Queue<QueueType>::isFull() {
    return count == SIZE;
}

int main() {
    Stack<int> intStack;
    intStack.init();
    
    cout << "Додаємо елементи в стек:\n";
    for(int i = 0; i < 5; i++) {
        cout << "Додаємо " << i << endl;
        intStack.push(i);
    }
    
    cout << "\nВидаляємо елементи зі стеку:\n";
    for(int i = 0; i < 5; i++) {
        cout << "Видалено " << intStack.pop() << endl;
    }
    
    Stack<char> charStack;
    charStack.init();
    
    cout << "\nДодаємо символи в стек:\n";
    for(char c = 'A'; c <= 'E'; c++) {
        cout << "Додаємо " << c << endl;
        charStack.push(c);
    }
    
    cout << "\nВидаляємо символи зі стеку:\n";
    for(int i = 0; i < 5; i++) {
        cout << "Видалено " << charStack.pop() << endl;
    }

    Queue<int> intQueue;
    intQueue.init();
    
    cout << "\nДодаємо елементи в чергу:\n";
    for(int i = 0; i < 5; i++) {
        cout << "Додаємо " << i << endl;
        intQueue.enqueue(i);
    }
    
    cout << "\nВидаляємо елементи з черги:\n";
    while(!intQueue.isEmpty()) {
        cout << "Видалено " << intQueue.dequeue() << endl;
    }
    
    return 0;
}