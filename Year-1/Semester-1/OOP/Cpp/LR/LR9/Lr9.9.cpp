#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;

class strtype {
    char *p;
    int len;
public:
    strtype(char *ptr) {
        len = strlen(ptr) + 1;
        p = new char[len];
        if (!p) {
            cout << "Allocation error\n";
            exit(1);
        }
        strcpy(p, ptr);
    }
    ~strtype() { delete[] p; }
    friend ostream &operator<<(ostream &stream, strtype &obj);
};

ostream &operator<<(ostream &stream, strtype &obj) {
    stream << obj.p;
    return stream;
}

int main() {
    strtype s1("This is a test."), s2("I hate C++!");
    cout << s1 << '\n' << s2;
    return 0;
}