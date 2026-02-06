#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ofstream out("phones.txt");
    if (!out) {
        cout << "File error\n";
        return 1;
    }
    string first, last, phone;
    for (int i = 0; i < 3; i++) {
        cout << "Enter first name, last name, phone: ";
        cin >> first >> last >> phone;
        out << first << " " << last << ", " << phone << '\n';
    }
    out.close();
    return 0;
}