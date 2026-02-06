#include <iostream>
#include <cmath>
using namespace std;

int main() {
    cout.setf(ios::right);
    cout.precision(5);
    cout.width(10);
    cout << "Number" << "  ";
    cout.width(10);
    cout << "ln" << "  ";
    cout.width(10);
    cout << "log10" << '\n';
    for (int i = 2; i <= 100; i++) {
        cout.width(10);
        cout << i;
        cout.width(10);
        cout << log(i);
        cout.width(10);
        cout << log10(i) << '\n';
    }
    return 0;
}