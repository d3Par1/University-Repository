#include <iostream>
using namespace std;

template <typename T>
int find(T object, T *list, int size) {
    for(int i = 0; i < size; i++) {
        if(list[i] == object) {
            return i;
        }
    }
    return -1; 
}

int main() {
    int intArray[] = {10, 20, 30, 40, 50};
    int intSize = 5;
    
    cout << "Масив цілих чисел: ";
    for(int i = 0; i < intSize; i++) {
        cout << intArray[i] << " ";
    }
    cout << endl;
    
    cout << "Індекс числа 30: " << find(30, intArray, intSize) << endl;
    cout << "Індекс числа 60: " << find(60, intArray, intSize) << endl;
    
    char charArray[] = {'a', 'b', 'c', 'd', 'e'};
    int charSize = 5;
    
    cout << "\nМасив символів: ";
    for(int i = 0; i < charSize; i++) {
        cout << charArray[i] << " ";
    }
    cout << endl;
    
    cout << "Індекс символу 'c': " << find('c', charArray, charSize) << endl;
    cout << "Індекс символу 'z': " << find('z', charArray, charSize) << endl;
    
    double doubleArray[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    int doubleSize = 5;
    
    cout << "\nМасив дійсних чисел: ";
    for(int i = 0; i < doubleSize; i++) {
        cout << doubleArray[i] << " ";
    }
    cout << endl;
    
    cout << "Індекс числа 3.3: " << find(3.3, doubleArray, doubleSize) << endl;
    cout << "Індекс числа 6.6: " << find(6.6, doubleArray, doubleSize) << endl;
    
    return 0;
}