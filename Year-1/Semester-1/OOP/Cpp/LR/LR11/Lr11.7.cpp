#include <iostream>
using namespace std;

class area {
    double dim1, dim2;
public:
    void setarea(double d1, double d2) {
        dim1 = d1;
        dim2 = d2;
    }
    void getdim(double &d1, double &d2) {
        d1 = dim1;
        d2 = dim2;
    }
    virtual double getarea() = 0; 
};

class rectangle: public area {
public:
    double getarea() {
        double d1, d2;
        getdim(d1, d2);
        return d1*d2;
    }
};

class triangle: public area {
public:
    double getarea() {
        double d1, d2;
        getdim(d1, d2);
        return 0.5 * d1 * d2;
    }
};

class base {
public:
    virtual void func() {
        cout << "Using base version of func()\n";
    }
    virtual ~base() {}
};

class derived1: public base {
public:
    void func() {
        cout << "Using derived1 version of func()\n";
    }
};

class derived2: public derived1 {
public:
};

int main() {
    cout << "Експеримент 1: Спроба створити об'єкт абстрактного класу area\n";   
    cout << "Створення об'єкта абстрактного класу неможливе через чисто віртуальну функцію getarea().\n";
    cout << "Повідомлення про помилку зазвичай вказує на те, що клас є абстрактним.\n\n";
    
    rectangle r;
    triangle t;
    
    r.setarea(3.3, 4.5);
    t.setarea(4.0, 5.0);
    
    area *p;
    p = &r;
    cout << "Rectangle has area: " << p->getarea() << "\n";
    
    p = &t;
    cout << "Triangle has area: " << p->getarea() << "\n\n";
    
    cout << "Експеримент 2: Виклик перевизначених віртуальних функцій\n";
    
    base *p1;
    base obj;
    derived1 obj1;
    derived2 obj2;
    
    p1 = &obj;
    p1->func(); 
    
    p1 = &obj1;
    p1->func(); 
    
    p1 = &obj2;
    p1->func(); 
    
    cout << "\nРезультат експерименту з видаленням func() з класу derived2:\n";
    cout << "При виклику p1->func(), де p1 вказує на об'єкт класу derived2,\n";
    cout << "використовується версія функції з батьківського класу derived1,\n";
    cout << "оскільки в derived2 немає власної версії цієї функції.\n\n";
    
    cout << "Що станеться при видаленні func() з класу derived1?\n";
    cout << "Якщо видалити функцію func() з класу derived1, то при виклику\n";
    cout << "p1->func(), де p1 вказує на об'єкт класу derived2, буде використана\n";
    cout << "версія функції з класу base, оскільки derived1 вже не перевизначає цю функцію.\n";
    cout << "Програма скомпілюється та запуститься без помилок, але поведінка буде іншою.\n";
    
    return 0;
}