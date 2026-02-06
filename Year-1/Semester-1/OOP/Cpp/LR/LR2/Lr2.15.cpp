#include <iostream>
#include <cstring>

using namespace std;

class strtype {
private:
    char* p;
    int len;

public:
    // Конструктор з параметром
    strtype(char* ptr) {
        len = strlen(ptr);
        p = new char[len + 1]; // +1 для нульового символу
        
        if (!p) {
            cout << "Помилка виділення пам'яті\n";
            exit(1);
        }
        
        strcpy(p, ptr);
    }
    
    // Деструктор
    ~strtype() {
        cout << "Видалення p\n";
        delete[] p;
    }
    
    // Метод для відображення рядка та його довжини
    void show() {
        cout << p << " - length: " << len << endl;
    }
};

int main() {
    // Створюємо два об'єкти з різними початковими значеннями
    strtype s1("This is a test."), s2("I like C++.");
    
    // Показуємо рядки та їх довжини
    s1.show();
    s2.show();
    
    return 0;
}