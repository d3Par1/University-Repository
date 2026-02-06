#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

class OutOfBoundsException : public runtime_error {
public:
    OutOfBoundsException(int index) : 
        runtime_error("Індекс " + to_string(index) + " виходить за межі масиву") {}
};

template <typename T>
class SafeArray {
private:
    T* array;
    int size;
    
public:
    SafeArray(int s) : size(s) {
        if (s <= 0) {
            throw invalid_argument("Розмір масиву повинен бути більше нуля");
        }
        array = new T[size];
        for (int i = 0; i < size; i++) {
            array[i] = T();
        }
    }
    
    ~SafeArray() {
        delete[] array;
    }
    
    T& operator[](int i) {
        if (i < 0 || i >= size) {
            throw OutOfBoundsException(i);
        }
        return array[i];
    }
    
    const T& operator[](int i) const {
        if (i < 0 || i >= size) {
            throw OutOfBoundsException(i);
        }
        return array[i];
    }
    
    int getSize() const {
        return size;
    }
};

int main() {
    try {
        const int SIZE = 5;
        SafeArray<int> intArray(SIZE);
        
        cout << "Заповнення масиву цілих чисел:" << endl;
        for (int i = 0; i < SIZE; i++) {
            intArray[i] = i * 10;
        }
        
        cout << "Значення елементів масиву:" << endl;
        for (int i = 0; i < SIZE; i++) {
            cout << "intArray[" << i << "] = " << intArray[i] << endl;
        }
        
        cout << "\nСпроба доступу за межами масиву (індекс " << SIZE << "):" << endl;
        cout << intArray[SIZE] << endl; 
    }
    catch (const OutOfBoundsException& e) {
        cout << "Виняток: " << e.what() << endl;
    }
    catch (const exception& e) {
        cout << "Інший виняток: " << e.what() << endl;
    }
    
    cout << "\nТепер спробуємо з масивом символів:" << endl;
    
    try {
        SafeArray<char> charArray(3);
        
        charArray[0] = 'A';
        charArray[1] = 'B';
        charArray[2] = 'C';
        
        for (int i = 0; i < 3; i++) {
            cout << "charArray[" << i << "] = " << charArray[i] << endl;
        }
        
        cout << "\nСпроба доступу з від'ємним індексом (-1):" << endl;
        cout << charArray[-1] << endl; 
    }
    catch (const OutOfBoundsException& e) {
        cout << "Виняток: " << e.what() << endl;
    }
    
    return 0;
}