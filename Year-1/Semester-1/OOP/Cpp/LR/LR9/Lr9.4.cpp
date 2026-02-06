#include <iostream>
using namespace std;

int main() {
    ios::fmtflags old_flags = cout.flags();
    cout.setf(ios::showbase | ios::hex, ios::basefield);
    cout << 100 << '\n';
    cout.flags(old_flags);
    cout << 100 << '\n';
    return 0;
}