#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    // Task 13
    cout << dec << "Decimal: " << 100 << '\n';
    cout << hex << "Hexadecimal: " << 100 << '\n';
    cout << oct << "Octal: " << 100 << '\n';
    cout << '\n';
    // Task 14
    cout << left << fixed << setprecision(2) << setfill('*') << setw(20) << 1000.5364 << '\n';
    return 0;
}