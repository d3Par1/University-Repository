#include <iostream>
#include <cstring>
using namespace std;

class mybase {
    char str[80];
public:
    mybase(char *s) { strcpy(str, s); }  // Виправлено синтаксис (прибрана зайва ;)
    char *get() { return str; }
};

class myderived : public mybase {
    int len;
public:
    // Конструктор myderived передає рядок конструктору mybase
    // та ініціалізує len довжиною рядка
    myderived(char *s) : mybase(s) {
        len = strlen(s);
    }
    
    int getlen() { return len; }
    void show() { cout << get() << '\n'; }
};

int main() {
    myderived obj("hello");
    
    obj.show();
    cout << obj.getlen() << '\n';
    
    // Додаткові тести
    myderived obj2("Hello, World!");
    cout << "\nДругий об'єкт:\n";
    obj2.show();
    cout << "Довжина: " << obj2.getlen() << '\n';
    
    return 0;
}