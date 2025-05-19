#include <iostream>
#include <iomanip>

using namespace std;

int main() {
    const double METERS_TO_FEET = 3.28084;
    const double FEET_TO_INCHES = 12.0;
    
    double meters;
    
    cout << "Програма для перетворення метрів в фути і дюйми\n";
    cout << "Введіть 0 для виходу з програми\n";
    
    do {
        cout << "\nВведіть кількість метрів: ";
        cin >> meters;
        
        if (meters == 0) {
            cout << "Дякуємо за використання програми!\n";
            break;
        }
        
        if (meters < 0) {
            cout << "Помилка! Довжина не може бути від'ємною. Спробуйте ще раз.\n";
            continue;
        }
        
        double feet = meters * METERS_TO_FEET;
        double inches = feet * FEET_TO_INCHES;
        
        cout << fixed << setprecision(2);
        cout << meters << " метрів = " << feet << " футів" << endl;
        cout << meters << " метрів = " << inches << " дюймів" << endl;
        
    } while (true);
    
    return 0;
}