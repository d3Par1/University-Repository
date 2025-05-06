#include <iostream>
#include <stdexcept>
using namespace std;

class StackException : public runtime_error {
public:
    StackException(const string& message) : runtime_error(message) {}
};

class StackOverflowException : public StackException {
public:
    StackOverflowException() : StackException("Переповнення стеку") {}
};

class StackUnderflowException : public StackException {
public:
    StackUnderflowException() : StackException("Спустошення стеку") {}
};

template <class StackType>
class Stack {
    static const int SIZE = 5; 
    StackType stck[SIZE]; 
    int tos; 
public:
    void init() {
        tos = 0;
    }
    
    void push(StackType obj) {
        if(tos == SIZE) {
            throw StackOverflowException();
        }
        stck[tos] = obj;
        tos++;
    }
    
    StackType pop() {
        if(tos == 0) {
            throw StackUnderflowException();
        }
        tos--;
        return stck[tos];
    }
    
    bool isEmpty() {
        return tos == 0;
    }
    
    bool isFull() {
        return tos == SIZE;
    }
    
    int size() {
        return tos;
    }
};

int main() {
    Stack<int> intStack;
    intStack.init();
    
    try {
        cout << "Додаємо елементи в стек:" << endl;
        for(int i = 0; i < 7; i++) { 
            cout << "Додаємо " << i << endl;
            intStack.push(i);
        }
    } catch (const StackException& e) {
        cout << "Виняток: " << e.what() << endl;
        cout << "Поточний розмір стеку: " << intStack.size() << endl;
    }
    
    cout << endl;
    
    try {
        cout << "Видаляємо елементи зі стеку:" << endl;
        while(true) {
            cout << "Видалено " << intStack.pop() << endl;
        }
    } catch (const StackException& e) {
        cout << "Виняток: " << e.what() << endl;
    }
    
    return 0;
}