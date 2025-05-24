#include <iostream>
using namespace std;

class A {
    int i;
public:
    A(int a) { i = a; }
    int getA() { return i; }
};

class B {
    int j;
public:
    B(int a) { j = a; }
    int getB() { return j; }
};

class C : public A, public B {
    int k;
public:
    // Конструктор C ініціалізує k та передає аргументи конструкторам A() і B()
    C(int x, int y, int z) : A(x), B(y) {
        k = z;
    }
    
    void show() {
        cout << "A::i = " << getA() << endl;
        cout << "B::j = " << getB() << endl;
        cout << "C::k = " << k << endl;
    }
};

int main() {
    // Створюємо об'єкт класу C
    C obj(10, 20, 30);
    
    cout << "Демонстрація множинного наслідування:\n";
    obj.show();
    
    // Інший приклад
    C obj2(100, 200, 300);
    cout << "\nДругий об'єкт:\n";
    obj2.show();
    
    return 0;
}