#include <iostream>
using namespace std;

class dynarray {
    int *ptr;
    int size;
    
public:
    // Конструктор
    dynarray(int s) {
        size = s;
        ptr = new int[size];
        // Ініціалізуємо нулями
        for(int i = 0; i < size; i++) {
            ptr[i] = 0;
        }
        cout << "Створено динамічний масив розміром " << size << " елементів\n";
    }
    
    // Конструктор копій
    dynarray(const dynarray &other) {
        size = other.size;
        ptr = new int[size];
        for(int i = 0; i < size; i++) {
            ptr[i] = other.ptr[i];
        }
        cout << "Створено копію масиву розміром " << size << " елементів\n";
    }
    
    // Деструктор
    ~dynarray() {
        delete[] ptr;
        cout << "Звільнено пам'ять для масиву розміром " << size << " елементів\n";
    }
    
    // Функція put() - повертає посилання на заданий елемент
    int& put(int i) {
        if(i < 0 || i >= size) {
            cout << "Помилка: індекс " << i << " виходить за межі масиву [0.." << size-1 << "]\n";
            // Повертаємо посилання на перший елемент у випадку помилки
            return ptr[0];
        }
        return ptr[i];
    }
    
    // Функція get() - повертає значення заданого елемента
    int get(int i) {
        if(i < 0 || i >= size) {
            cout << "Помилка: індекс " << i << " виходить за межі масиву [0.." << size-1 << "]\n";
            return 0;  // Повертаємо 0 у випадку помилки
        }
        return ptr[i];
    }
    
    // Перевантажений оператор присвоєння
    dynarray& operator=(const dynarray &other) {
        // Перевірка на самоприсвоєння
        if(this == &other) {
            return *this;
        }
        
        // Звільняємо стару пам'ять
        delete[] ptr;
        
        // Копіюємо дані з іншого об'єкта
        size = other.size;
        ptr = new int[size];
        for(int i = 0; i < size; i++) {
            ptr[i] = other.ptr[i];
        }
        
        cout << "Виконано присвоєння масиву розміром " << size << " елементів\n";
        return *this;
    }
    
    // Допоміжні функції
    int get_size() const { return size; }
    
    void fill_with_value(int value) {
        for(int i = 0; i < size; i++) {
            ptr[i] = value;
        }
    }
    
    void fill_with_sequence() {
        for(int i = 0; i < size; i++) {
            ptr[i] = i + 1;
        }
    }
    
    void print() {
        cout << "Масив [" << size << "]: ";
        for(int i = 0; i < size; i++) {
            cout << ptr[i];
            if(i < size - 1) cout << ", ";
        }
        cout << "\n";
    }
    
    void print_info() {
        cout << "Розмір: " << size << ", Адреса: " << ptr << "\n";
    }
};

int main() {
    cout << "ДЕМОНСТРАЦІЯ КЛАСУ ДИНАМІЧНОГО МАСИВУ:\n";
    cout << "======================================\n\n";
    
    // Створення масивів
    cout << "1. СТВОРЕННЯ МАСИВІВ:\n";
    cout << "====================\n";
    dynarray arr1(5);
    dynarray arr2(3);
    
    cout << "\nІнформація про створені масиви:\n";
    cout << "arr1: "; arr1.print_info();
    cout << "arr2: "; arr2.print_info();
    cout << "\n";
    
    // Заповнення масивів
    cout << "2. ЗАПОВНЕННЯ МАСИВІВ:\n";
    cout << "=====================\n";
    
    // Заповнення через put()
    cout << "Заповнення arr1 через put():\n";
    for(int i = 0; i < arr1.get_size(); i++) {
        arr1.put(i) = (i + 1) * 10;
    }
    arr1.print();
    
    // Заповнення arr2
    cout << "\nЗаповнення arr2 послідовністю:\n";
    arr2.fill_with_sequence();
    arr2.print();
    
    cout << "\n";
    
    // Тестування get() та put()
    cout << "3. ТЕСТУВАННЯ GET() ТА PUT():\n";
    cout << "=============================\n";
    
    cout << "Читання елементів arr1 через get():\n";
    for(int i = 0; i < arr1.get_size(); i++) {
        cout << "arr1[" << i << "] = " << arr1.get(i) << "\n";
    }
    
    cout << "\nЗміна елемента через put():\n";
    cout << "arr1.put(2) було: " << arr1.get(2) << "\n";
    arr1.put(2) = 999;
    cout << "arr1.put(2) стало: " << arr1.get(2) << "\n";
    arr1.print();
    
    // Тестування контролю кордонів
    cout << "\n4. ТЕСТУВАННЯ КОНТРОЛЮ КОРДОНІВ:\n";
    cout << "===============================\n";
    
    cout << "Спроба доступу до неіснуючого індексу:\n";
    int value = arr1.get(10);  // Помилка
    cout << "Повернене значення: " << value << "\n";
    
    cout << "Спроба встановлення неіснуючого індексу:\n";
    arr1.put(10) = 555;  // Помилка, але змінить перший елемент
    arr1.print();
    
    // Демонстрація оператора присвоєння
    cout << "\n5. ДЕМОНСТРАЦІЯ ОПЕРАТОРА ПРИСВОЄННЯ:\n";
    cout << "====================================\n";
    
    cout << "До присвоєння:\n";
    cout << "arr1: "; arr1.print();
    cout << "arr2: "; arr2.print();
    
    cout << "\nІнформація про адреси:\n";
    cout << "arr1: "; arr1.print_info();
    cout << "arr2: "; arr2.print_info();
    
    // Присвоєння
    cout << "\nВиконуємо arr2 = arr1:\n";
    arr2 = arr1;
    
    cout << "\nПісля присвоєння:\n";
    cout << "arr1: "; arr1.print();
    cout << "arr2: "; arr2.print();
    
    cout << "\nІнформація про адреси після присвоєння:\n";
    cout << "arr1: "; arr1.print_info();
    cout << "arr2: "; arr2.print_info();
    cout << "Зверніть увагу: різні адреси пам'яті!\n";
    
    // Перевірка незалежності копій
    cout << "\n6. ПЕРЕВІРКА НЕЗАЛЕЖНОСТІ КОПІЙ:\n";
    cout << "===============================\n";
    
    cout << "Змінюємо arr1.put(0) = 777:\n";
    arr1.put(0) = 777;
    
    cout << "arr1: "; arr1.print();
    cout << "arr2: "; arr2.print();
    cout << "arr2 не змінився - копії незалежні!\n";
    
    // Тестування самоприсвоєння
    cout << "\n7. ТЕСТУВАННЯ САМОПРИСВОЄННЯ:\n";
    cout << "============================\n";
    
    cout << "Виконуємо arr1 = arr1 (самоприсвоєння):\n";
    arr1 = arr1;
    cout << "arr1: "; arr1.print();
    cout << "Самоприсвоєння оброблено коректно!\n";
    
    // Ланцюгове присвоєння
    cout << "\n8. ЛАНЦЮГОВЕ ПРИСВОЄННЯ:\n";
    cout << "=======================\n";
    
    dynarray arr3(4);
    arr3.fill_with_value(888);
    
    cout << "До ланцюгового присвоєння:\n";
    cout << "arr1: "; arr1.print();
    cout << "arr2: "; arr2.print();
    cout << "arr3: "; arr3.print();
    
    // Ланцюгове присвоєння arr1 = arr2 = arr3
    cout << "\nВиконуємо arr1 = arr2 = arr3:\n";
    arr1 = arr2 = arr3;
    
    cout << "Після ланцюгового присвоєння:\n";
    cout << "arr1: "; arr1.print();
    cout << "arr2: "; arr2.print();
    cout << "arr3: "; arr3.print();
    
    cout << "\n9. ВАЖЛИВІСТЬ ПРАВИЛЬНОГО ОПЕРАТОРА ПРИСВОЄННЯ:\n";
    cout << "===============================================\n";
    cout << "1. Запобігає витоку пам'яті\n";
    cout << "2. Створює справжні копії даних\n";
    cout << "3. Обробляє самоприсвоєння\n";
    cout << "4. Дозволяє ланцюгове присвоєння\n";
    cout << "5. Забезпечує правило 'глибокого копіювання'\n";
    
    cout << "\nПрограма завершується, деструктори звільнять пам'ять...\n";
    
    return 0;
}