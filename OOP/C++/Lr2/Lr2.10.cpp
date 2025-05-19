#include <iostream>

using namespace std;

// Перевантажена функція min() для символів
char min(char a, char b) {
    return (a < b) ? a : b;
}

// Перевантажена функція min() для цілих чисел
int min(int a, int b) {
    return (a < b) ? a : b;
}

// Перевантажена функція min() для чисел з плаваючою точкою подвійної точності
double min(double a, double b) {
    return (a < b) ? a : b;
}

int main() {
    // Тестування функції min() для різних типів даних
    
    // Символи
    char ch1 = 'A', ch2 = 'Z';
    cout << "min('" << ch1 << "', '" << ch2 << "') = '" << min(ch1, ch2) << "'" << endl;
    
    // Цілі числа
    int i1 = 42, i2 = 19;
    cout << "min(" << i1 << ", " << i2 << ") = " << min(i1, i2) << endl;
    
    // Числа з плаваючою точкою
    double d1 = 3.14, d2 = 2.71;
    cout << "min(" << d1 << ", " << d2 << ") = " << min(d1, d2) << endl;
    
    return 0;
}