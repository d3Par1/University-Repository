#include <iostream>
#include <ctime>
using namespace std;

ostream &tdm(ostream &stream) {
    time_t t = time(nullptr);
    stream << ctime(&t);
    return stream;
}

int main() {
    cout << tdm;
    return 0;
}