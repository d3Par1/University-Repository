#include <iostream>
#include <ctime>
#include <string>

using namespace std;

class time_and_date {
private:
    time_t current_time;
    tm* timeinfo;
    
public:
    // Конструктор, який отримує поточний час та дату
    time_and_date() {
        current_time = time(NULL);
        timeinfo = localtime(&current_time);
    }
    
    // Метод для виведення часу та дати на екран
    void display() {
        char buffer[80];
        
        // Форматуємо дату та час
        strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", timeinfo);
        
        cout << "Поточний час та дата: " << buffer << endl;
        
        // Виводимо додаткову інформацію
        cout << "День тижня: ";
        
        switch(timeinfo->tm_wday) {
            case 0: cout << "Неділя"; break;
            case 1: cout << "Понеділок"; break;
            case 2: cout << "Вівторок"; break;
            case 3: cout << "Середа"; break;
            case 4: cout << "Четвер"; break;
            case 5: cout << "П'ятниця"; break;
            case 6: cout << "Субота"; break;
        }
        cout << endl;
        
        cout << "День року: " << timeinfo->tm_yday + 1 << endl;
    }
};

int main() {
    cout << "Демонстрація роботи класу time_and_date:" << endl;
    
    // Створюємо об'єкт класу time_and_date
    time_and_date td;
    
    // Відображаємо поточний час та дату
    td.display();
    
    return 0;
}