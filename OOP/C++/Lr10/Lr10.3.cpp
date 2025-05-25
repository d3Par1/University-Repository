#include <iostream>
using namespace std;

class date {
    char d[9]; // mm/dd/yy
public:
    friend ostream &operator<<(ostream &stream, date obj);
    friend istream &operator>>(istream &stream, date &obj);
};

ostream &operator<<(ostream &stream, date obj) {
    stream << obj.d;
    return stream;
}

istream &operator>>(istream &stream, date &obj) {
    stream.getline(obj.d, 9);
    return stream;
}

int main() {
    date d;
    cout << "Enter date (mm/dd/yy): ";
    cin >> d;
    cout << d << '\n';
    return 0;
}