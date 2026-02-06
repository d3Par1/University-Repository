#include <iostream>
#include <iomanip>
using namespace std;

ostream &sethex(ostream &stream) {
    stream << hex << uppercase << showbase;
    return stream;
}

ostream &reset(ostream &stream) {
    stream << dec << nouppercase << noshowbase;
    return stream;
}

int main() {
    cout << sethex << 100 << '\n';
    cout << reset << 100 << '\n';
    return 0;
}