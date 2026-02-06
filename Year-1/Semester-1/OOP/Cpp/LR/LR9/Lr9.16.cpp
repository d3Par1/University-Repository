#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    ios::fmtflags f = cout.flags();
    cout << hex << showbase << 100 << '\n';
    cout.flags(f);
    cout << 100 << '\n';
    return 0;
}