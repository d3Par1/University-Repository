#include <iostream>
using namespace std;

class array {
    int nums[10];
public:
    array();
    void set(int n[10]);
    void show();
    array operator+(array obj2);
    array operator-(array obj2);
    bool operator==(array obj2);
    
    // Оператор ++ як функція-член класу (префіксна форма)
    array& operator++();
    
    // Оператор -- як дружня функція (префіксна форма)
    friend array& operator--(array& obj);
};

array::array() { 
    for (int i = 0; i < 10; i++) 
        nums[i] = 0; 
}

void array::set(int *n) { 
    for (int i = 0; i < 10; i++) 
        nums[i] = n[i]; 
}

void array::show() {
    for (int i = 0; i < 10; i++)
        cout << nums[i] << ' ';
    cout << "\n";
}

// Оператор додавання
array array::operator+(array obj2) {
    array temp;
    for (int i = 0; i < 10; i++) {
        temp.nums[i] = nums[i] + obj2.nums[i];
    }
    return temp;
}

// Оператор віднімання
array array::operator-(array obj2) {
    array temp;
    for (int i = 0; i < 10; i++) {
        temp.nums[i] = nums[i] - obj2.nums[i];
    }
    return temp;
}

// Оператор порівняння
bool array::operator==(array obj2) {
    for (int i = 0; i < 10; i++) {
        if (nums[i] != obj2.nums[i]) {
            return false;
        }
    }
    return true;
}

// Префіксний оператор ++ (функція-член)
// Збільшує всі елементи масиву на 1
array& array::operator++() {
    for (int i = 0; i < 10; i++) {
        nums[i]++;
    }
    return *this;
}

// Префіксний оператор -- (дружня функція)
// Зменшує всі елементи масиву на 1
array& operator--(array& obj) {
    for (int i = 0; i < 10; i++) {
        obj.nums[i]--;
    }
    return obj;
}

int main() {
    array obj1, obj2, obj3;
    int i[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    obj1.set(i);
    obj2.set(i);
    
    cout << "Початковий obj1: ";
    obj1.show();
    
    // Використання префіксного оператора ++
    ++obj1;
    cout << "Після ++obj1: ";
    obj1.show();
    
    // Використання префіксного оператора --
    --obj1;
    cout << "Після --obj1: ";
    obj1.show();
    
    // Тестування інших операторів
    obj3 = obj1 + obj2;
    cout << "obj1 + obj2: ";
    obj3.show();
    
    obj3 = obj1 - obj2;
    cout << "obj1 - obj2: ";
    obj3.show();
    
    if (obj1 == obj2) 
        cout << "obj1 equals obj2\n";
    else 
        cout << "obj1 does not equal obj2\n";
    
    return 0;
}