#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    cout << setiosflags(ios::showbase | ios::hex) << 100 << '\n';
    return 0;
}