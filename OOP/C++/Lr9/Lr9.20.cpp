#include <iostream>
#include <ctime>
using namespace std;

class watch {
    time_t t;
public:
    watch() : t(time(nullptr)) {}
    friend ostream &operator<<(ostream &stream, watch obj);
};

ostream &operator<<(ostream &stream, watch obj) {
    stream << ctime(&obj.t);
    return stream;
}

int main() {
    watch w;
    cout << w;
    return 0;
}