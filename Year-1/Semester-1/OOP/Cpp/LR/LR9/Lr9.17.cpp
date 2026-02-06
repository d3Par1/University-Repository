#include <iostream>
using namespace std;

class pwr {
    int base, exponent;
    double result;
public:
    pwr(int b, int e) : base(b), exponent(e) {
        result = 1;
        for (; e; e--) result *= base;
    }
    friend ostream &operator<<(ostream &stream, pwr obj);
    friend istream &operator>>(istream &stream, pwr &obj);
};

ostream &operator<<(ostream &stream, pwr obj) {
    stream << obj.base << "^" << obj.exponent << " = " << obj.result;
    return stream;
}

istream &operator>>(istream &stream, pwr &obj) {
    cout << "Enter base and exponent: ";
    stream >> obj.base >> obj.exponent;
    obj.result = 1;
    for (int e = obj.exponent; e > 0; e--) obj.result *= obj.base;
    return stream;
}

int main() {
    pwr p(0, 0);
    cin >> p;
    cout << p << '\n';
    return 0;
}