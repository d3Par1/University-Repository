#include <iostream>
using namespace std;

class mybase {
    int a, b;
public:
    int c;
    void setab(int i, int j) { a = i; b = j; }
    void getab(int &i, int &j) { i = a; j = b; }
};

class derived1 : public mybase {
    // При public наслідуванні:
    // - public члени базового класу залишаються public
    // - protected члени залишаються protected
    // - private члени недоступні
};

class derived2 : private mybase {
    // При private наслідуванні:
    // - всі public та protected члени базового класу стають private
    // - private члени недоступні
};

int main() {
    derived1 obj1;
    derived2 obj2;
    int i, j;
    
    /* Аналіз правильності інструкцій:
     * 
     * a) obj1.getab(i, j); - ПРАВИЛЬНО
     *    getab() є public в mybase і залишається public в derived1
     *    через public наслідування
     * 
     * b) obj2.getab(i, j); - НЕПРАВИЛЬНО
     *    getab() стає private в derived2 через private наслідування,
     *    тому недоступна ззовні класу
     * 
     * c) obj1.c = 10; - ПРАВИЛЬНО
     *    c є public в mybase і залишається public в derived1
     *    через public наслідування
     * 
     * d) obj2.c = 10; - НЕПРАВИЛЬНО
     *    c стає private в derived2 через private наслідування,
     *    тому недоступна ззовні класу
     */
    
    // Демонстрація правильних інструкцій:
    cout << "Демонстрація правильних операцій:\n";
    
    // a) Правильно - public метод доступний через public наслідування
    obj1.setab(5, 10);
    obj1.getab(i, j);
    cout << "a) obj1.getab(): i = " << i << ", j = " << j << endl;
    
    // c) Правильно - public змінна доступна через public наслідування
    obj1.c = 10;
    cout << "c) obj1.c = " << obj1.c << endl;
    
    // Наступні рядки викликають помилки компіляції:
    // obj2.getab(i, j);  // Помилка: getab() є private в derived2
    // obj2.c = 10;       // Помилка: c є private в derived2
    
    return 0;
}