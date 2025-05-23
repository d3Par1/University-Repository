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
        cout << "Створено копію динамічного масиву розміром " << size << "\n";
    }
    
    // Деструктор
    ~dynarray() {
        delete[] ptr;
        cout << "Звільнено пам'ять динамічного масиву розміром " << size << "\n";
    }
    
    // Перевантаження оператора [] (замінює get() та put())
    int& operator[](int i) {
        if(i < 0 || i >= size) {
            cout << "Помилка: індекс " << i << " виходить за межі масиву [0.." << size-1 << "]\n";
            cout << "Повертаємо посилання на перший елемент\n";
            return ptr[0];
        }
        return ptr[i];
    }
    
    // Константна версія оператора []
    const int& operator[](int i) const {
        if(i < 0 || i >= size) {
            cout << "Помилка: індекс " << i << " виходить за межі масиву [0.." << size-1 << "]\n";
            cout << "Повертаємо посилання на перший елемент\n";
            return ptr[0];
        }
        return ptr[i];
    }
    
    // Перевантажений оператор присвоєння
    dynarray& operator=(const dynarray &other) {
        if(this == &other) return *this;
        
        delete[] ptr;
        size = other.size;
        ptr = new int[size];
        for(int i = 0; i < size; i++) {
            ptr[i] = other.ptr[i];
        }
        
        cout << "Присвоєння динамічного масиву розміром " << size << "\n";
        return *this;
    }
    
    // Допоміжні методи
    int get_size() const { return size; }
    
    void print() const {
        cout << "DynArray[" << size << "]: ";
        for(int i = 0; i < size; i++) {
            cout << ptr[i];
            if(i < size - 1) cout << ", ";
        }
        cout << "\n";
    }
    
    void fill_with_value(int value) {
        for(int i = 0; i < size; i++) {
            ptr[i] = value;
        }
    }
    
    void fill_with_sequence(int start = 1) {
        for(int i = 0; i < size; i++) {
            ptr[i] = start + i;
        }
    }
    
    void print_info() const {
        cout << "Розмір: " << size << ", Адреса пам'яті: " << ptr << "\n";
    }
    
    // Додаткові корисні методи
    int sum() const {
        int total = 0;
        for(int i = 0; i < size; i++) {
            total += ptr[i];
        }
        return total;
    }
    
    int max() const {
        if(size == 0) return 0;
        int maximum = ptr[0];
        for(int i = 1; i < size; i++) {
            if(ptr[i] > maximum) {
                maximum = ptr[i];
            }
        }
        return maximum;
    }
    
    int min() const {
        if(size == 0) return 0;
        int minimum = ptr[0];
        for(int i = 1; i < size; i++) {
            if(ptr[i] < minimum) {
                minimum = ptr[i];
            }
        }
        return minimum;
    }
    
    void sort() {
        // Простий bubble sort
        for(int i = 0; i < size - 1; i++) {
            for(int j = 0; j < size - i - 1; j++) {
                if(ptr[j] > ptr[j + 1]) {
                    int temp = ptr[j];
                    ptr[j] = ptr[j + 1];
                    ptr[j + 1] = temp;
                }
            }
        }
    }
};

int main() {
    cout << "ДЕМОНСТРАЦІЯ ОПЕРАТОРА [] ДЛЯ ДИНАМІЧНОГО МАСИВУ:\n";
    cout << "=================================================\n\n";
    
    // Створення динамічних масивів
    cout << "1. СТВОРЕННЯ ДИНАМІЧНИХ МАСИВІВ:\n";
    cout << "===============================\n";
    
    dynarray arr1(7);
    dynarray arr2(5);
    
    cout << "\nІнформація про створені масиви:\n";
    cout << "arr1: "; arr1.print_info();
    cout << "arr2: "; arr2.print_info();
    
    cout << "\nПочаткові значення:\n";
    cout << "arr1: "; arr1.print();
    cout << "arr2: "; arr2.print();
    
    // Заповнення через оператор []
    cout << "\n2. ЗАПОВНЕННЯ ЧЕРЕЗ ОПЕРАТОР []:\n";
    cout << "===============================\n";
    
    cout << "Заповнення arr1 квадратами чисел:\n";
    for(int i = 0; i < arr1.get_size(); i++) {
        arr1[i] = (i + 1) * (i + 1);  // 1, 4, 9, 16, 25, 36, 49
    }
    arr1.print();
    
    cout << "\nЗаповнення arr2 числами Фібоначчі:\n";
    if(arr2.get_size() > 0) arr2[0] = 1;
    if(arr2.get_size() > 1) arr2[1] = 1;
    for(int i = 2; i < arr2.get_size(); i++) {
        arr2[i] = arr2[i-1] + arr2[i-2];
    }
    arr2.print();
    
    // Читання через оператор []
    cout << "\n3. ЧИТАННЯ ЧЕРЕЗ ОПЕРАТОР []:\n";
    cout << "============================\n";
    
    cout << "Елементи arr1:\n";
    for(int i = 0; i < arr1.get_size(); i++) {
        cout << "arr1[" << i << "] = " << arr1[i] << "\n";
    }
    
    cout << "\nПерший та останній елементи arr2:\n";
    cout << "arr2[0] = " << arr2[0] << " (перший)\n";
    cout << "arr2[" << arr2.get_size()-1 << "] = " 
         << arr2[arr2.get_size()-1] << " (останній)\n";
    
    // Використання в лівій частині присвоєння
    cout << "\n4. МОДИФІКАЦІЯ ЕЛЕМЕНТІВ:\n";
    cout << "========================\n";
    
    cout << "arr1 до модифікації: "; arr1.print();
    
    // Подвоєння всіх елементів
    cout << "Подвоєння всіх елементів arr1:\n";
    for(int i = 0; i < arr1.get_size(); i++) {
        arr1[i] *= 2;
    }
    arr1.print();
    
    // Зміна конкретних елементів
    cout << "\nЗміна конкретних елементів:\n";
    cout << "arr1[0] було: " << arr1[0];
    arr1[0] = 999;
    cout << ", стало: " << arr1[0] << "\n";
    
    cout << "arr1[3] було: " << arr1[3];
    arr1[3] += 100;
    cout << ", стало: " << arr1[3] << "\n";
    
    arr1.print();
    
    // Контроль кордонів
    cout << "\n5. КОНТРОЛЬ КОРДОНІВ:\n";
    cout << "====================\n";
    
    cout << "Коректний доступ:\n";
    cout << "arr1[0] = " << arr1[0] << "\n";
    cout << "arr1[" << arr1.get_size()-1 << "] = " << arr1[arr1.get_size()-1] << "\n";
    
    cout << "\nНекоректний доступ:\n";
    int bad_value = arr1[100];  // Помилка, але повертає arr1[0]
    cout << "arr1[100] = " << bad_value << " (повернено значення першого елемента)\n";
    
    // Спроба запису в некоректний індекс
    cout << "Спроба arr1[50] = 777:\n";
    arr1[50] = 777;  // Помилка, але змінить arr1[0]
    cout << "arr1[0] тепер: " << arr1[0] << " (змінився перший елемент)\n";
    
    // Робота з константним масивом
    cout << "\n6. РОБОТА З КОНСТАНТНИМ МАСИВОМ:\n";
    cout << "===============================\n";
    
    const dynarray const_arr = arr2;  // Копіювання
    cout << "Константний масив: "; const_arr.print();
    
    cout << "Читання з константного масиву:\n";
    for(int i = 0; i < const_arr.get_size(); i++) {
        cout << "const_arr[" << i << "] = " << const_arr[i] << "\n";
    }
    
    // Наступний рядок НЕ скомпілюється:
    // const_arr[0] = 555;  // ПОМИЛКА! Не можна змінювати константний об'єкт
    
    // Практичні операції з масивом
    cout << "\n7. ПРАКТИЧНІ ОПЕРАЦІЇ:\n";
    cout << "====================\n";
    
    dynarray numbers(10);
    
    // Заповнення випадковими числами (симуляція)
    int test_data[] = {45, 23, 78, 12, 90, 34, 67, 89, 56, 41};
    cout << "Заповнення тестовими даними:\n";
    for(int i = 0; i < numbers.get_size(); i++) {
        numbers[i] = test_data[i];
    }
    numbers.print();
    
    // Статистика
    cout << "\nСтатистика масиву:\n";
    cout << "Сума елементів: " << numbers.sum() << "\n";
    cout << "Максимальний елемент: " << numbers.max() << "\n";
    cout << "Мінімальний елемент: " << numbers.min() << "\n";
    cout << "Середнє значення: " << double(numbers.sum()) / numbers.get_size() << "\n";
    
    // Пошук елемента
    cout << "\nПошук елемента 78:\n";
    int search_value = 78;
    int found_index = -1;
    for(int i = 0; i < numbers.get_size(); i++) {
        if(numbers[i] == search_value) {
            found_index = i;
            break;
        }
    }
    
    if(found_index != -1) {
        cout << "Елемент " << search_value << " знайдено на індексі " << found_index << "\n";
    } else {
        cout << "Елемент " << search_value << " не знайдено\n";
    }
    
    // Сортування
    cout << "\nСортування масиву:\n";
    cout << "До сортування: "; numbers.print();
    numbers.sort();
    cout << "Після сортування: "; numbers.print();
    
    // Фільтрація (парні числа)
    cout << "\n8. ФІЛЬТРАЦІЯ ДАНИХ:\n";
    cout << "===================\n";
    
    cout << "Парні числа в масиві:\n";
    for(int i = 0; i < numbers.get_size(); i++) {
        if(numbers[i] % 2 == 0) {
            cout << "numbers[" << i << "] = " << numbers[i] << " (парне)\n";
        }
    }
    
    // Трансформація (збільшити всі елементи на 10)
    cout << "\nТрансформація (збільшення на 10):\n";
    cout << "До: "; numbers.print();
    for(int i = 0; i < numbers.get_size(); i++) {
        numbers[i] += 10;
    }
    cout << "Після: "; numbers.print();
    
    // Порівняння масивів
    cout << "\n9. ПОРІВНЯННЯ МАСИВІВ:\n";
    cout << "=====================\n";
    
    dynarray copy1 = arr2;
    dynarray copy2 = arr2;
    
    cout << "copy1: "; copy1.print();
    cout << "copy2: "; copy2.print();
    
    // Перевірка на рівність
    bool are_equal = true;
    if(copy1.get_size() == copy2.get_size()) {
        for(int i = 0; i < copy1.get_size(); i++) {
            if(copy1[i] != copy2[i]) {
                are_equal = false;
                break;
            }
        }
    } else {
        are_equal = false;
    }
    
    cout << "Масиви " << (are_equal ? "однакові" : "різні") << "\n";
    
    // Змінюємо один елемент
    copy2[0] = 999;
    cout << "\nПісля зміни copy2[0] = 999:\n";
    cout << "copy1: "; copy1.print();
    cout << "copy2: "; copy2.print();
    cout << "Тепер масиви різні\n";
    
    cout << "\n10. ПЕРЕВАГИ ОПЕРАТОРА [] НАД GET()/PUT():\n";
    cout << "=========================================\n";
    
    cout << "ПЕРЕВАГИ:\n";
    cout << "+ Природний та інтуїтивний синтаксис\n";
    cout << "+ Можна використовувати в лівій частині присвоєння\n";
    cout << "+ Зручність у циклах та алгоритмах\n";
    cout << "+ Сумісність з STL алгоритмами\n";
    cout << "+ Підтримка константних об'єктів\n";
    cout << "+ Одна функція для читання та запису\n";
    
    cout << "\nПОРІВНЯННЯ СИНТАКСИСУ:\n";
    cout << "Старий спосіб: arr.put(i) = value; value = arr.get(i);\n";
    cout << "Новий спосіб:  arr[i] = value;     value = arr[i];\n";
    
    return 0;
}