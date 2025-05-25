#include <iostream>
using namespace std;

int main() {
    cout.setf(ios::showpoint | ios::scientific | ios::uppercase);
    cout << 123.456 << '\n';
    return 0;
}