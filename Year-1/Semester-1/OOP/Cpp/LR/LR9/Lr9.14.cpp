#include <iostream>
using namespace std;

int main() {
    cout.setf(ios::left);
    cout.setf(ios::fixed);
    cout.precision(2);
    cout.fill('*');
    cout.width(20);
    cout << 1000.5364 << '\n';
    return 0;
}