#include <iostream>

using namespace std;

class mybox {
private:
    double length;  // Довжина паралелепіпеда
    double width;   // Ширина паралелепіпеда
    double height;  // Висота паралелепіпеда
    double vol;     // Об'єм паралелепіпеда
    
public:
    // Конструктор, який приймає три параметри
    mybox(double l, double w, double h) {
        length = l;
        width = w;
        height = h;
        
        // Розрахунок об'єму
        vol = length * width * height;
    }
    
    // Метод для виведення об'єму на екран
    void volume() {
        cout << "Розміри паралелепіпеда: " << length << " x " << width << " x " << height << endl;
        cout << "Об'єм паралелепіпеда: " << vol << endl;
    }
};

int main() {
    cout << "Демонстрація роботи класу mybox:" << endl;
    
    // Створюємо кілька об'єктів з різними розмірами
    mybox box1(10.0, 5.0, 2.0);
    mybox box2(3.5, 7.2, 4.8);
    mybox box3(2.0, 2.0, 2.0);
    
    // Виводимо об'єми на екран
    cout << "Box1:" << endl;
    box1.volume();
    
    cout << "\nBox2:" << endl;
    box2.volume();
    
    cout << "\nBox3:" << endl;
    box3.volume();
    
    return 0;
}