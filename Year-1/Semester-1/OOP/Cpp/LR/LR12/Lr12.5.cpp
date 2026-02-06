#include <iostream>
#include <stdexcept>
using namespace std;

double divide(double a, double b) {
    if (b == 0) {
        throw runtime_error("Помилка: ділення на нуль неможливе!");
    }
    return a/b;
}

int main() {
    double num1, num2, result;
    
    cout << "Введіть перше число (чисельник): ";
    cin >> num1;
    
    cout << "Введіть друге число (знаменник): ";
    cin >> num2;
    
    try {
        result = divide(num1, num2);
        cout << "Результат ділення " << num1 << " на " << num2 << " = " << result << endl;
    } catch (const exception& e) {
        cout << e.what() << endl;
    }
    
    cout << "Програма завершена." << endl;
    
    return 0;
}