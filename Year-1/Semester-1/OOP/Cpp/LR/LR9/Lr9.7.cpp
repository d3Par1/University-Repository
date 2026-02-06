#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
using namespace std;

void center(char *s) {
    int len = strlen(s);
    int spaces = (80 - len) / 2;
    cout << setw(spaces + len) << s << '\n';
}

int main() {
    // Task 5
    cout << right;
    cout << setprecision(5);
    cout << setw(10) << "Number" << "  ";
    cout << setw(10) << "ln" << "  ";
    cout << setw(10) << "log10" << '\n';
    for (int i = 2; i <= 100; i++) {
        cout << setw(10) << i;
        cout << setw(10) << log(i);
        cout << setw(10) << log10(i) << '\n';
    }
    cout << '\n';
    // Task 6
    center("Hello");
    center("Centered Text");
    return 0;
}