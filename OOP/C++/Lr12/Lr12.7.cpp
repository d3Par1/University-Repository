#include <iostream>
using namespace std;

template <typename T>
T sum(T arr[], int size) {
    T total = T();
    
    for (int i = 0; i < size; i++) {
        total += arr[i];
    }
    
    return total;
}

int main() {
    int intArray[] = {1, 2, 3, 4, 5};
    int intSize = sizeof(intArray) / sizeof(intArray[0]);
    
    cout << "Масив цілих чисел: ";
    for (int i = 0; i < intSize; i++) {
        cout << intArray[i] << " ";
    }
    cout << endl;
    
    cout << "Сума елементів: " << sum(intArray, intSize) << endl;

    double doubleArray[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    int doubleSize = sizeof(doubleArray) / sizeof(doubleArray[0]);
    
    cout << "\nМасив дійсних чисел: ";
    for (int i = 0; i < doubleSize; i++) {
        cout << doubleArray[i] << " ";
    }
    cout << endl;
    
    cout << "Сума елементів: " << sum(doubleArray, doubleSize) << endl;
    char charArray[] = {'a', 'b', 'c', 'd', 'e'};
    int charSize = sizeof(charArray) / sizeof(charArray[0]);
    
    cout << "\nМасив символів: ";
    for (int i = 0; i < charSize; i++) {
        cout << charArray[i] << " ";
    }
    cout << endl;
    
    cout << "Сума кодів ASCII: " << (int)sum(charArray, charSize) << endl;
    
    return 0;
}