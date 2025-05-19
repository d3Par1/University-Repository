#include <iostream>
#include <cmath>

using namespace std;

int main() {
    int base, exponent;
    
    cout << "Програма для обчислення степеня числа" << endl;
    
    // Ввід основи
    cout << "Введіть основу: ";
    cin >> base;
    
    // Ввід показника степеня
    cout << "Введіть показник степеня: ";
    cin >> exponent;
    
    // Обчислення результату з використанням функції pow()
    double result = pow(base, exponent);
    
    // Вивід результату
    cout << base << "^" << exponent << " = " << result << endl;
    
    return 0;
}