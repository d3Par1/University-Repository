#include <iostream>
#include <chrono>
#include <thread>
#include <string>

using namespace std;

// Перевантажена функція sleep() для цілих чисел
void sleep(int seconds) {
    cout << "Зупинка на " << seconds << " секунд..." << endl;
    this_thread::sleep_for(chrono::seconds(seconds));
    cout << "Завершено!" << endl;
}

// Перевантажена функція sleep() для рядків
void sleep(const string& seconds_str) {
    try {
        int seconds = stoi(seconds_str);
        cout << "Зупинка на " << seconds << " секунд (з рядка \"" << seconds_str << "\")..." << endl;
        this_thread::sleep_for(chrono::seconds(seconds));
        cout << "Завершено!" << endl;
    } catch (const exception& e) {
        cout << "Помилка при перетворенні рядка в число: " << e.what() << endl;
    }
}

int main() {
    cout << "Демонстрація перевантаженої функції sleep()" << endl;
    
    // Використовуємо sleep() з цілим числом
    sleep(2);
    
    // Використовуємо sleep() з рядком
    sleep("3");
    
    // Тестування з некоректним значенням
    sleep("не число");
    
    cout << "Програма завершена" << endl;
    return 0;
}