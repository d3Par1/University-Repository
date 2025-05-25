#include <iostream>
#include <cstring>
using namespace std;

class mybase {
    char str[80];
public:
    mybase(char *s) {
        strcpy(str, s);
    }
    char *get() {
        return str;
    }
};

class myderived : public mybase {
    int len;
public:
    myderived(char *s) : mybase(s) {
        len = strlen(s);
    }
    int getlen() {
        return len;
    }
    void show() {
        cout << get() << '\n';
    }
};

int main() {
    myderived obj("hello");
    obj.show();
    cout << obj.getlen() << '\n';
    return 0;
}