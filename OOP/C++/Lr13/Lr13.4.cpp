#include <iostream>
using namespace std;

void f(const double &i) {
    
    double& non_const_i = const_cast<double&>(i);
    non_const_i = 100;
}

void danger_example1() {
    cout << "\nНебезпечний приклад 1:\n";
    
    const double value = 10.0;
    cout << "Початкове значення константи: " << value << endl;
    
    double& ref = const_cast<double&>(value);
    ref = 20.0;
    
    cout << "Значення після модифікації через const_cast: " << value << endl;
    cout << "Значення ref: " << ref << endl;
    cout << "Примітка: Результати можуть бути непередбачуваними, оскільки ми\n";
    cout << "модифікуємо об'єкт, який був визначений як const!\n";
}

void danger_example2() {
    cout << "\nНебезпечний приклад 2:\n";
    cout << "Уявімо, що ми працюємо з API функцією, яка приймає тільки неконстантний вказівник:\n";
    cout << "void some_api_function(char* str) { ... }\n\n";
    
    const char* const_str = "Hello";
    
    cout << "Початковий рядок: " << const_str << endl;
    cout << "Використання const_cast для виклику API:\n";
    cout << "char* non_const_str = const_cast<char*>(const_str);\n";
    cout << "some_api_function(non_const_str); // Небезпечно!\n\n";
    
    cout << "Проблема: Якщо API спробує модифікувати рядок, це призведе до невизначеної\n";
    cout << "поведінки, оскільки рядкові літерали зазвичай розміщуються у області пам'яті тільки для читання.\n";
}

int main() {
    double x = 98.6;
    cout << "Початкове значення x: " << x << endl;
    
    f(x);
    
    cout << "Значення x після виклику f(): " << x << endl;
    
    danger_example1();
    danger_example2();
    
    cout << "\nВисновки щодо використання const_cast:\n";
    cout << "1. Оператор const_cast порушує принцип const-коректності C++.\n";
    cout << "2. Модифікація справді константних об'єктів призводить до невизначеної поведінки.\n";
    cout << "3. Використовуйте const_cast лише коли ви впевнені, що об'єкт не був\n";
    cout << "   визначений як const, але переданий у функцію як const з інших причин.\n";
    cout << "4. Краще переробити дизайн коду, щоб уникнути необхідності const_cast.\n";
    
    return 0;
}