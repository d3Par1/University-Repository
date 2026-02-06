#include <iostream>
using namespace std;

class Shape {
public:
    virtual void print() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
public:
    void print() override {
        cout << "Circle\n";
    }
};

class Square : public Shape {
public:
    void print() override {
        cout << "Square\n";
    }
};

class Triangle : public Shape {
public:
    void print() override {
        cout << "Triangle\n";
    }
};

class NullShape : public Shape {
public:
    void print() override {
        cout << "NullShape\n";
    }
};

Shape* generator() {
    switch(rand() % 4) {
        case 0: return new Circle;
        case 1: return new Square;
        case 2: return new Triangle;
        case 3: return new NullShape;
    }
    return nullptr;
}

int main() {
    Shape *p;
    
    srand(static_cast<unsigned int>(time(nullptr)));
    
    for(int i=0; i<10; i++) {
        p = generator();
        
        if(dynamic_cast<NullShape*>(p) == nullptr) {
            cout << "Object " << i << ": ";
            p->print();
        } else {
            cout << "Object " << i << ": (NullShape suppressed)\n";
        }
        
        delete p; 
    }
    
    return 0;
}