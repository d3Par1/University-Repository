#include <iostream>
using namespace std;

class array {
    int nums[10];
public:
    array() {
        for (int i = 0; i < 10; i++) nums[i] = 0;
    }
    void set(int n[10]) {
        for (int i = 0; i < 10; i++) nums[i] = n[i];
    }
    void show() {
        for (int i = 0; i < 10; i++) {
            cout << nums[i] << ' ';
        }
        cout << endl;
    }
    array operator++() {
        array temp = *this;
        for (int i = 0; i < 10; i++) {
            nums[i]++;
        }
        return temp;
    }
    friend array operator--(array &obj);
};

array operator--(array &obj) {
    array temp = obj;
    for (int i = 0; i < 10; i++) {
        obj.nums[i]--;
    }
    return temp;
}

int main() {
    array obj1;
    int i[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    obj1.set(i);
    cout << "Original array: ";
    obj1.show();
    array obj2 = ++obj1;
    cout << "After ++obj1: ";
    obj1.show();
    cout << "Returned value: ";
    obj2.show();
    obj2 = --obj1;
    cout << "After --obj1: ";
    obj1.show();
    cout << "Returned value: ";
    obj2.show();
    return 0;
}