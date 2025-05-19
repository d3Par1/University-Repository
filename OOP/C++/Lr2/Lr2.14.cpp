#include <iostream>
#include <ctime>

using namespace std;

class stopwatch {
private:
    time_t startTime;  // Час запуску секундоміра
    time_t stopTime;   // Час зупинки секундоміра
    bool running;      // Прапорець, що вказує, чи секундомір запущено
    time_t createTime; // Час створення об'єкта

public:
    // Конструктор - встановлює секундомір в 0 і запам'ятовує час створення
    stopwatch() {
        startTime = 0;
        stopTime = 0;
        running = false;
        createTime = time(NULL);
        cout << "Секундомір створено і встановлено в 0" << endl;
    }
    
    // Деструктор - виводить час, що пройшов з моменту створення об'єкта
    ~stopwatch() {
        time_t now = time(NULL);
        cout << "Секундомір видалено. Загальний час існування: " 
             << difftime(now, createTime) << " секунд" << endl;
    }
    
    // Запуск секундоміра
    void start() {
        if (!running) {
            startTime = time(NULL);
            running = true;
            cout << "Секундомір запущено" << endl;
        } else {
            cout << "Секундомір вже запущено" << endl;
        }
    }
    
    // Зупинка секундоміра
    void stop() {
        if (running) {
            stopTime = time(NULL);
            running = false;
            cout << "Секундомір зупинено" << endl;
        } else {
            cout << "Секундомір вже зупинено" << endl;
        }
    }
    
    // Показ пройденого часу
    void show() {
        if (running) {
            time_t now = time(NULL);
            cout << "Пройшло часу: " << difftime(now, startTime) << " секунд" << endl;
        } else if (startTime != 0) {
            cout << "Пройшло часу: " << difftime(stopTime, startTime) << " секунд" << endl;
        } else {
            cout << "Секундомір ще не запускався" << endl;
        }
    }
};

int main() {
    cout << "Демонстрація роботи класу stopwatch:" << endl;
    
    // Створюємо об'єкт секундоміра
    stopwatch sw;
    
    // Запускаємо секундомір
    sw.start();
    
    // Затримка
    cout << "Очікування 2 секунди..." << endl;
    time_t delay = time(NULL) + 2;
    while (time(NULL) < delay) {
        // Чекаємо
    }
    
    // Показуємо поточний час
    sw.show();
    
    // Ще затримка
    cout << "Очікування ще 3 секунди..." << endl;
    delay = time(NULL) + 3;
    while (time(NULL) < delay) {
        // Чекаємо
    }
    
    // Зупиняємо секундомір
    sw.stop();
    
    // Показуємо загальний час
    sw.show();
    
    // Спроба повторно зупинити секундомір
    sw.stop();
    
    // Затримка перед завершенням програми
    cout << "Очікування 1 секунду перед виходом..." << endl;
    delay = time(NULL) + 1;
    while (time(NULL) < delay) {
        // Чекаємо
    }
    
    // Деструктор викличеться автоматично при завершенні функції main
    return 0;
}