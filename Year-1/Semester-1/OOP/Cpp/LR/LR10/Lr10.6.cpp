#include <iostream>
using namespace std;

istream &skipchar1(istream &stream) {
    char c;
    int count = 0;
    while (stream.get(c)) {
        if (count % 5 < 3) cout << c;
        count++;
    }
    return stream;
}

istream &skipchar2(istream &stream) {
    char c;
    int count = 0;
    while (stream.get(c)) {
        if (count % 4 != 0) cout << c;
        count++;
    }
    return stream;
}

int main() {
    cout << "Enter text for skipchar1: ";
    cin >> skipchar1;
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "Enter text for skipchar2: ";
    cin >> skipchar2;
    return 0;
}