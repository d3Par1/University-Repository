#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    // Зберігаємо початковий символ заповнення
    char old_fill = cout.fill();
    
    cout << "Демонстрація форматованого виведення:" << endl;
    cout << "=====================================" << endl << endl;
    
    // Встановлюємо символ заповнення ':' (двокрапка)
    cout.fill(':');
    
    // Встановлюємо ширину поля 30 символів
    cout.width(30);
    
    // Виводимо речення "I hate C++"
    cout << "I hate C++" << endl;
    
    cout << endl;
    cout << "Додаткові приклади:" << endl;
    
    // Вирівнювання по правому краю (за замовчуванням)
    cout.fill(':');
    cout.width(30);
    cout << right << "I hate C++" << endl;
    
    // Вирівнювання по лівому краю
    cout.fill(':');
    cout.width(30);
    cout << left << "I hate C++" << endl;
    
    // Вирівнювання по центру (internal - для чисел)
    cout.fill(':');
    cout.width(30);
    cout << internal << "I hate C++" << endl;
    
    cout << endl;
    cout << "Приклад з числами:" << endl;
    
    // Виведення числа з символом заповнення
    cout.fill(':');
    cout.width(30);
    cout << right << 12345 << endl;
    
    cout.fill(':');
    cout.width(30);
    cout << left << -12345 << endl;
    
    // Повертаємо початковий символ заповнення
    cout.fill(old_fill);
    
    return 0;
}