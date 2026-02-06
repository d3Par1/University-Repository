#include <iostream>
#include <string>
using namespace std;

template <class T1, class T2>
class Pair {
public:
    T1 first;
    T2 second;
    
    Pair() {}
    
    Pair(T1 f, T2 s) : first(f), second(s) {}
    
    void display() {
        cout << "(" << first << ", " << second << ")";
    }
};

template <class T1, class T2>
class Stack {
    static const int SIZE = 100;
    Pair<T1, T2> stck[SIZE]; 
    int tos;
public:
    void init() {
        tos = 0;
    }
    
    void push(T1 first, T2 second) {
        if(tos == SIZE) {
            cout << "Стек переповнений.\n";
            return;
        }
        stck[tos] = Pair<T1, T2>(first, second);
        tos++;
    }
    
    Pair<T1, T2> pop() {
        if(tos == 0) {
            cout << "Стек порожній.\n";
            return Pair<T1, T2>();
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
};

int main() {
    Stack<int, string> stack;
    stack.init();
    
    cout << "Додаємо пари у стек:\n";
    stack.push(1, "Один");
    stack.push(2, "Два");
    stack.push(3, "Три");
    stack.push(4, "Чотири");
    stack.push(5, "П'ять");
    
    cout << "\nВидаляємо пари зі стеку:\n";
    while(!stack.isEmpty()) {
        Pair<int, string> pair = stack.pop();
        cout << "Видалено: ";
        pair.display();
        cout << endl;
    }
    
    Stack<char, double> charDoubleStack;
    charDoubleStack.init();
    
    cout << "\nДодаємо пари іншого типу у стек:\n";
    charDoubleStack.push('A', 1.1);
    charDoubleStack.push('B', 2.2);
    charDoubleStack.push('C', 3.3);
    
    cout << "\nВидаляємо пари зі стеку:\n";
    while(!charDoubleStack.isEmpty()) {
        Pair<char, double> pair = charDoubleStack.pop();
        cout << "Видалено: ";
        pair.display();
        cout << endl;
    }
    
    return 0;
}