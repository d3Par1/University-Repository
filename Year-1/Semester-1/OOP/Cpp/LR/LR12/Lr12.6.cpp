#include <iostream>
#include <map>
#include <stdexcept>
using namespace std;

template <typename T>
T mostFrequent(T arr[], int size) {
    if (size <= 0) {
        throw runtime_error("Масив порожній або має недійсний розмір");
    }
    
    // Використовуємо map для підрахунку частоти кожного елемента
    map<T, int> freq;
    
    // Підраховуємо частоту кожного елемента
    for (int i = 0; i < size; i++) {
        freq[arr[i]]++;
    }
    
    // Знаходимо елемент з найвищою частотою
    T mostFreqElement = arr[0];
    int maxFreq = 0;
    
    for (const auto& pair : freq) {
        if (pair.second > maxFreq) {
            mostFreqElement = pair.first;
            maxFreq = pair.second;
        }
    }
    
    return mostFreqElement;
}

int main() {
    try {
        // Тестування з цілими числами
        int intArray[] = {1, 2, 3, 2, 2, 4, 5, 2, 6, 7};
        int intSize = sizeof(intArray) / sizeof(intArray[0]);
        
        cout << "Масив цілих чисел: ";
        for (int i = 0; i < intSize; i++) {
            cout << intArray[i] << " ";
        }
        cout << endl;
        
        cout << "Найчастіше зустрічається елемент: " 
             << mostFrequent(intArray, intSize) << endl;
        
        // Тестування з символами
        char charArray[] = {'a', 'b', 'c', 'b', 'd', 'b', 'e'};
        int charSize = sizeof(charArray) / sizeof(charArray[0]);
        
        cout << "\nМасив символів: ";
        for (int i = 0; i < charSize; i++) {
            cout << charArray[i] << " ";
        }
        cout << endl;
        
        cout << "Найчастіше зустрічається елемент: " 
             << mostFrequent(charArray, charSize) << endl;
        
        // Тестування з дійсними числами
        double doubleArray[] = {1.1, 2.2, 3.3, 2.2, 4.4, 2.2, 5.5};
        int doubleSize = sizeof(doubleArray) / sizeof(doubleArray[0]);
        
        cout << "\nМасив дійсних чисел: ";
        for (int i = 0; i < doubleSize; i++) {
            cout << doubleArray[i] << " ";
        }
        cout << endl;
        
        cout << "Найчастіше зустрічається елемент: " 
             << mostFrequent(doubleArray, doubleSize) << endl;
    }
    catch (const exception& e) {
        cout << "Помилка: " << e.what() << endl;
    }
    
    return 0;
}