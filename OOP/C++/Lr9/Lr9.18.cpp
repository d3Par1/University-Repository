#include <iostream>
using namespace std;

#define SIZE 10

class stack {
    char stck[SIZE];
    int tos;
public:
    stack() : tos(0) {}
    void push(char ch) {
        if (tos == SIZE) {
            cout << "Stack is full\n";
            return;
        }
        stck[tos++] = ch;
    }
    char pop() {
        if (tos == 0) {
            cout << "Stack is empty\n";
            return 0;
        }
        return stck[--tos];
    }
    friend ostream &operator<<(ostream &stream, stack obj);
};

ostream &operator<<(ostream &stream, stack obj) {
    if (obj.tos == 0) {
        stream << "Stack is empty";
    } else {
        stream << "Stack contents: ";
        for (int i = 0; i < obj.tos; i++) {
            stream << obj.stck[i] << ' ';
        }
    }
    return stream;
}

int main() {
    stack s;
    s.push('A');
    s.push('B');
    s.push('C');
    cout << s << '\n';
    return 0;
}