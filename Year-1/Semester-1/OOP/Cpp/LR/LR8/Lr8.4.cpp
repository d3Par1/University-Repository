#include <iostream>
using namespace std;

class mybase {
    int a, b;
public:
    int c;
    void setab(int i, int j) {
        a = i;
        b = j;
    }
    void getab(int &i, int &j) {
        i = a;
        j = b;
    }
};

class derived1 : public mybase {
};

class derived2 : private mybase {
public:
    using mybase::setab;
    using mybase::getab;
    using mybase::c;
};

int main() {
    derived1 obj1;
    derived2 obj2;
    int i, j;
    obj1.getab(i, j); // a) Correct: public inheritance allows access to public members
    // obj2.getab(i, j); // b) Correct: made public via 'using' in derived2
    obj1.c = 10; // c) Correct: c is public in mybase, accessible via public inheritance
    obj2.c = 10; // d) Correct: c is made public via 'using' in derived2
    cout << "All statements are correct after proper 'using' declarations in derived2.\n";
    return 0;
}