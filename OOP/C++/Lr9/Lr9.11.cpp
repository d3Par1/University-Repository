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
    friend istream &operator>>(istream &stream, strtype &obj);
};

ostream &operator<<(ostream &stream, strtype &obj) {
    stream << obj.p;
    return stream;
}

istream &operator>>(istream &stream, strtype &obj) {
    char temp[256];
    stream.getline(temp, 256);
    delete[] obj.p;
    obj.len = strlen(temp) + 1;
    obj.p = new char[obj.len];
    if (!obj.p) {
        cout << "Allocation error\n";
        exit(1);
    }
    strcpy(obj.p, temp);
    return stream;
}

int main() {
    strtype s1("Initial");
    cout << "Enter a string: ";
    cin >> s1;
    cout << s1 << '\n';
    return 0;
}