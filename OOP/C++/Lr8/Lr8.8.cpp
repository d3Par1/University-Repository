#include <iostream>
using namespace std;

class A {
    int i;
public:
    A(int a) : i(a) {}
};

class B {
    int j;
public:
    B(int a) : j(a) {}
};

class C : public A, public B {
    int k;
public:
    C(int a, int b, int c) : A(a), B(b), k(c) {}
};

int main() {
    C obj(1, 2, 3);
    return 0;
}