#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

bool isValidInput(const string& input, bool checkDecimalPlaces = false) {
    bool hasDecimal = false;
    int digitsAfterDecimal = 0;
    
    // Пуста стрічка є невалідною
    if (input.empty()) return false;
    
    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] == '.') {
            if (hasDecimal) return false; // Більше однієї десяткової крапки
            hasDecimal = true;
        } else if (isdigit(input[i])) {
            if (hasDecimal) digitsAfterDecimal++;
        } else {
            return false; // Невалідний символ
        }
    }
    
    // Якщо перевіряємо кількість десяткових знаків
    if (checkDecimalPlaces && hasDecimal && digitsAfterDecimal > 2) {
        return false; // Більше 2 цифр після десяткової крапки
    }
    
    // Перевіряємо чи число додатнє
    double value = stod(input);
    return value >= 0;
}

int main() {
    string hoursStr, rateStr;
    double hours, hourlyRate, totalSalary;
    
    cout << "Програма для розрахунку заробітної плати персоналу\n";
    
    // Ввід годин
    do {
        cout << "Введіть кількість відпрацьованих годин: ";
        cin >> hoursStr;
        
        if (!isValidInput(hoursStr)) {
            cout << "Помилка! Кількість годин має бути додатнім числом.\n";
        } else {
            hours = stod(hoursStr);
            break;
        }
    } while (true);
    
    // Ввід погодинної оплати
    do {
        cout << "Введіть погодинну оплату: ";
        cin >> rateStr;
        
        if (!isValidInput(rateStr, true)) {
            cout << "Помилка! Оплата має бути додатнім числом у форматі XXX.XX (не більше двох цифр після коми).\n";
        } else {
            hourlyRate = stod(rateStr);
            break;
        }
    } while (true);
    
    // Обчислення і вивід сумарної зарплати
    totalSalary = hours * hourlyRate;
    cout << fixed << setprecision(2);
    cout << "Сумарна заробітна плата: " << totalSalary << endl;
   
    return 0;
    
}