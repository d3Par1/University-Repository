#include <iostream>
using namespace std;

class box {
    int size;
public:
    box(int s) : size(s) {}
    friend ostream &operator<<(ostream &stream, box obj);
};

ostream &operator<<(ostream &stream, box obj) {
    for (int i = 0; i < obj.size; i++) {
        for (int j = 0; j < obj.size; j++) {
            stream << '*';
        }
        stream << '\n';
    }
    return stream;
}

int main() {
    box b(5);
    cout << b;
    return 0;
}