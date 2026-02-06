#include <iostream>
using namespace std;

template <class T>
class coord {
    T x, y; 
public:
    coord() { x=0; y=0; cout << "Constructing A." << "\n"; }
    coord(T i, T j) { x=i; y=j; cout << "Constructing B." << "\n"; }
    void get_xy(T &i, T &j) { i=x; j=y; }
    ~coord() { cout << "Destructing." << "\n"; }

    coord operator +(const coord &obj) {
        coord temp;
        temp.x = x + obj.x;
        temp.y = y + obj.y;
        return temp;
    }
};

int main() {
    coord<int> obj1(10, 10), obj2(5, 3), obj3;
    int x, y;
    
    obj3 = obj1 + obj2; 
    obj3.get_xy(x, y);
    cout << "(obj1+obj2) X: " << x << ", Y: " << y << "\n";
    
    coord<double> d1(10.5, 10.8), d2(5.1, 3.2), d3;
    double dx, dy;
    
    d3 = d1 + d2;
    d3.get_xy(dx, dy);
    cout << "(d1+d2) X: " << dx << ", Y: " << dy << "\n";
    
    return 0;
}