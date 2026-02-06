#include <iostream>
using namespace std;

class number {
    int value, greatest_divisor;
    int find_greatest_divisor(int n) {
        if (n == 0) return 0;
        for (int i = n / 2; i >= 1; i--) {
            if (n % i == 0) return i;
        }
        return 1;
    }
public:
    number(int v = 0) : value(v), greatest_divisor(find_greatest_divisor(v)) {}
    friend ostream &operator<<(ostream &stream, number obj);
    friend istream &operator>>(istream &stream, number &obj);
};

ostream &operator<<(ostream &stream, number obj) {
    stream << "Value: " << obj.value << ", Greatest Divisor: " << obj.greatest_divisor;
    return stream;
}

istream &operator>>(istream &stream, number &obj) {
    stream >> obj.value;
    obj.greatest_divisor = obj.find_greatest_divisor(obj.value);
    return stream;
}

int main() {
    number n;
    cout << "Enter an integer: ";
    cin >> n;
    cout << n << '\n';
    return 0;
}