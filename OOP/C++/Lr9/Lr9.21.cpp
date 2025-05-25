#include <iostream>
using namespace std;

class ft_to_cms {
    double feet, cms;
public:
    void set(double f) {
        feet = f;
        cms = f * 30.48;
    }
    friend ostream &operator<<(ostream &stream, ft_to_cms obj);
    friend istream &operator>>(istream &stream, ft_to_cms &obj);
};

ostream &operator<<(ostream &stream, ft_to_cms obj) {
    stream << obj.feet << " ft = " << obj.cms << " cm";
    return stream;
}

istream &operator>>(istream &stream, ft_to_cms &obj) {
    cout << "Enter feet: ";
    stream >> obj.feet;
    obj.cms = obj.feet * 30.48;
    return stream;
}

int main() {
    ft_to_cms obj;
    cin >> obj;
    cout << obj << '\n';
    return 0;
}