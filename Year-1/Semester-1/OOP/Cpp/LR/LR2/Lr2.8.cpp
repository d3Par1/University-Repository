#include <iostream>
#include <cmath>

using namespace std;

// Перевантажена функція для цілого числа
int myroot(int num) {
    return static_cast<int>(sqrt(num));
}

// Перевантажена функція для довгого цілого числа
long myroot(long num) {
    return static_cast<long>(sqrt(num));
}

// Перевантажена функція для числа з плаваючою точкою подвійної точності
double myroot(double num) {
    return sqrt(num);
}

int main() {
    int intVal = 25;
    long longVal = 10000L;
    double doubleVal = 12.25;
    
    cout << "Квадратний корінь з " << intVal << " (int) = " << myroot(intVal) << endl;
    cout << "Квадратний корінь з " << longVal << " (long) = " << myroot(longVal) << endl;
    cout << "Квадратний корінь з " << doubleVal << " (double) = " << myroot(doubleVal) << endl;
    
    return 0;
}