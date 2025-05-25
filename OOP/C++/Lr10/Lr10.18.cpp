#include <iostream>
#include <cctype>
using namespace std;

istream &findalpha(istream &stream) {
    char c;
    while (stream.get(c)) {
        if (isalpha(c)) {
            stream.putback(c);
            break;
        }
    }
    return stream;
}

int main() {
    char c;
    cout << "Enter text: ";
    cin >> findalpha;
    cin.get(c);
    cout << "First alphabetic char: " << c << '\n';
    return 0;
}