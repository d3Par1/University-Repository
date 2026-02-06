#include <iostream>
#include <iomanip>

// Клас масиву з функціональністю введення/виведення
template <typename T, int SIZE>
class Array {
private:
    T data[SIZE];
    
public:
    // Конструктор за замовчуванням
    Array() {
        for (int i = 0; i < SIZE; i++) {
            data[i] = T();
        }
    }
    
    // Оператор доступу до елементів
    T& operator[](int index) {
        if (index < 0 || index >= SIZE) {
            std::cerr << "Індекс виходить за межі масиву!" << std::endl;
            exit(1);
        }
        return data[index];
    }
    
    const T& operator[](int index) const {
        if (index < 0 || index >= SIZE) {
            std::cerr << "Індекс виходить за межі масиву!" << std::endl;
            exit(1);
        }
        return data[index];
    }
    
    // Оператор введення
    friend std::istream& operator>>(std::istream& is, Array<T, SIZE>& arr) {
        for (int i = 0; i < SIZE; i++) {
            is >> arr.data[i];
        }
        return is;
    }
    
    // Оператор виведення
    friend std::ostream& operator<<(std::ostream& os, const Array<T, SIZE>& arr) {
        for (int i = 0; i < SIZE; i++) {
            os << std::setw(8) << arr.data[i];
            if ((i + 1) % 5 == 0) os << std::endl;
        }
        if (SIZE % 5 != 0) os << std::endl;
        return os;
    }
    
    // Копіювання з іншого масиву
    void copyFrom(const Array<T, SIZE>& source) {
        for (int i = 0; i < SIZE; i++) {
            data[i] = source.data[i];
        }
    }
};

int main() {
    const int SIZE = 10;
    Array<int, SIZE> arr1, arr2;
    
    // Ініціалізуємо перший масив
    std::cout << "Ініціалізуємо перший масив..." << std::endl;
    for (int i = 0; i < SIZE; i++) {
        arr1[i] = i * 10;
    }
    
    // Відображаємо перший масив
    std::cout << "\nВміст першого масиву:\n" << arr1;
    
    // Копіюємо arr1 в arr2
    std::cout << "Копіюємо перший масив у другий..." << std::endl;
    arr2.copyFrom(arr1);
    
    // Відображаємо другий масив
    std::cout << "\nВміст другого масиву після копіювання:\n" << arr2;
    
    // Змінюємо другий масив
    std::cout << "Модифікуємо другий масив..." << std::endl;
    for (int i = 0; i < SIZE; i++) {
        arr2[i] = arr2[i] + 5;
    }
    
    // Відображаємо обидва масиви для доказу незалежності
    std::cout << "\nПерший масив (не змінився):\n" << arr1;
    std::cout << "\nДругий масив (модифікований):\n" << arr2;
    
    // Демонстрація введення з консолі
    Array<int, 3> arr3;
    std::cout << "\nВведіть 3 числа для третього масиву: ";
    std::cin >> arr3;
    
    std::cout << "Ви ввели:\n" << arr3;
    
    return 0;
}