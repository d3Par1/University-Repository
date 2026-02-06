#include <iostream>
using namespace std;

class mybase {
protected:  // Змінено з private на protected
    int a, b;
public:
    int c;
    void setab(int i, int j) { a = i; b = j; }
    void getab(int &i, int &j) { i = a; j = b; }
};

class derived1 : public mybase {
public:
    // Тепер можемо отримати доступ до a і b всередині класу
    void showProtected() {
        cout << "В derived1 (public): a = " << a << ", b = " << b << endl;
    }
};

class derived2 : private mybase {
public:
    // Публічний метод для доступу до setab()
    void setab(int i, int j) { mybase::setab(i, j); }
    // Також можемо отримати доступ до a і b всередині класу
    void showProtected() {
        cout << "В derived2 (private): a = " << a << ", b = " << b << endl;
    }
};

int main() {
    derived1 obj1;
    derived2 obj2;
    int i, j;
    
    cout << "Завдання 5 - Аналіз змін при використанні protected:\n\n";
    
    /* Відповідь на питання:
     * НІ, жодна з відповідей НЕ змінилася б!
     * 
     * Причина: protected члени недоступні ззовні класу (з main()),
     * незалежно від типу наслідування. 
     * 
     * Protected члени доступні лише:
     * - всередині самого класу
     * - всередині похідних класів
     * 
     * Але НЕ доступні ззовні класу (з main() або інших функцій).
     * 
     * Тому відповіді залишаються такими ж:
     * a) obj1.getab(i, j); - ПРАВИЛЬНО
     * b) obj2.getab(i, j); - НЕПРАВИЛЬНО  
     * c) obj1.c = 10; - ПРАВИЛЬНО
     * d) obj2.c = 10; - НЕПРАВИЛЬНО
     */
    
    // Демонстрація роботи з protected членами
    obj1.setab(100, 200);
    obj1.getab(i, j);
    cout << "obj1 після setab(100, 200): i = " << i << ", j = " << j << endl;
    
    // Показуємо, що protected члени доступні всередині похідних класів
    obj1.showProtected();
    
    obj2.setab(300, 400);  // Помилка: setab() є private в derived2
    // obj2.showProtected();  // Але цей метод public і працює
    
    return 0;
}