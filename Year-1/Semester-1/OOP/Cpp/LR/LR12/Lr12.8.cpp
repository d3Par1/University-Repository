#include <iostream>
using namespace std;

template <typename T>
class BubbleSort {
private:
    T* array; 
    int size; 
    
public:
    BubbleSort(T arr[], int arrSize) : size(arrSize) {
        array = new T[size];
        for (int i = 0; i < size; i++) {
            array[i] = arr[i];
        }
    }
    
    ~BubbleSort() {
        delete[] array;
    }
    
    void sort() {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (array[j] > array[j + 1]) {
                    T temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
    }
    
    void display() {
        for (int i = 0; i < size; i++) {
            cout << array[i] << " ";
        }
        cout << endl;
    }
    
    T* getSortedArray() {
        return array;
    }
};

int main() {
    int intArray[] = {64, 34, 25, 12, 22, 11, 90};
    int intSize = sizeof(intArray) / sizeof(intArray[0]);
    
    BubbleSort<int> intSorter(intArray, intSize);
    cout << "Початковий масив цілих чисел: ";
    intSorter.display();
    
    intSorter.sort();
    cout << "Відсортований масив цілих чисел: ";
    intSorter.display();
    
    char charArray[] = {'z', 'y', 'x', 'w', 'v', 'u', 't'};
    int charSize = sizeof(charArray) / sizeof(charArray[0]);
    
    BubbleSort<char> charSorter(charArray, charSize);
    cout << "\nПочатковий масив символів: ";
    charSorter.display();
    
    charSorter.sort();
    cout << "Відсортований масив символів: ";
    charSorter.display();
    
    double doubleArray[] = {6.4, 3.4, 2.5, 1.2, 2.2, 1.1, 9.0};
    int doubleSize = sizeof(doubleArray) / sizeof(doubleArray[0]);
    
    BubbleSort<double> doubleSorter(doubleArray, doubleSize);
    cout << "\nПочатковий масив дійсних чисел: ";
    doubleSorter.display();
    
    doubleSorter.sort();
    cout << "Відсортований масив дійсних чисел: ";
    doubleSorter.display();
    
    return 0;
}