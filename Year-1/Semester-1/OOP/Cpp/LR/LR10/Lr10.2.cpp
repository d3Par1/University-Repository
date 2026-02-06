#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    // Version 1: ios functions
    cout.setf(ios::fixed);
    cout.precision(4);
    cout << 10.0 / 3 << '\n';
    // Version 2: manipulators
    cout << fixed << setprecision(4) << 10.0 / 3 << '\n';
    return 0;
}