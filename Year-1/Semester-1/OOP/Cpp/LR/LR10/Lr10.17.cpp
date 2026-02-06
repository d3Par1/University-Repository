#include <iostream>
#include <iomanip>
using namespace std;

ostream &tab3(ostream &stream) {
    stream << "\t\t\t" << setw(20);
    return stream;
}

int main() {
    cout << tab3 << "Test" << '\n';
    return 0;
}