#include <iostream>
using namespace std;

int main() {
    cout.setf(ios::dec, ios::basefield);
    cout << "Decimal: " << 100 << '\n';
    cout.setf(ios::hex, ios::basefield);
    cout << "Hexadecimal: " << 100 << '\n';
    cout.setf(ios::oct, ios::basefield);
    cout << "Octal: " << 100 << '\n';
    return 0;
}