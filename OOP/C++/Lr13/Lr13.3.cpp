#include <iostream>
#include <typeinfo>
#include <cmath>
using namespace std;

template <class T> class Num {
public:
    T x;
    Num(T i) { x = i; }
    virtual T get_val() { return x; }
};

template <class T> class Square: public Num <T> {
public:
    Square(T i): Num<T>(i) {}
    T get_val() override { return this->x * this->x; }
};

template <class T> class Sqr_root: public Num <T> {
public:
    Sqr_root(T i): Num<T>(i) {}
    T get_val() override { return sqrt((double)this->x); }
};

int main() {
    
    cout << "Демонстрація правильного використання dynamic_cast:\n";
    
    Num<double> *ptrb = new Square<double>(100.0);
    cout << "ptrb->get_val(): " << ptrb->get_val() << endl; 
    
    Square<double> *ptrd = dynamic_cast<Square<double>*>(ptrb);

    if (ptrd) {
        cout << "Cast successful! ptrd->get_val(): " << ptrd->get_val() << endl;
    } else {
        cout << "Cast failed!" << endl;
    }
    
    delete ptrb;
    
    cout << "\nАналіз проблемного фрагмента коду:\n";
    cout << "Num<int> *ptrb;\n";
    cout << "Square<double> *ptrd;\n";
    cout << "// ...\n";
    cout << "ptrd = dynamic_cast<Num<int>>(ptrb); // Некоректно!\n\n";
    
    cout << "Цей фрагмент коду не буде працездатним з наступних причин:\n";
    cout << "1. Синтаксична помилка: неправильний запис dynamic_cast.\n";
    cout << "   Правильно: dynamic_cast<Num<int>*>(ptrb)\n\n";
    
    cout << "2. Навіть з виправленим синтаксисом, перетворення не буде дійсним, оскільки:\n";
    cout << "   - Неможливо перетворити Num<int>* в Square<double>*\n";
    cout << "   - Різниця в типах шаблонів (int та double)\n";
    cout << "   - Неправильний напрямок перетворення (від базового до похідного)\n\n";
    
    cout << "3. Правильний вигляд перетворення від базового до похідного класу:\n";
    cout << "   Num<double>* ptrb = new Square<double>(10.0);\n";
    cout << "   Square<double>* ptrd = dynamic_cast<Square<double>*>(ptrb);\n";
    
    return 0;
}