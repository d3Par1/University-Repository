#include <iostream>

class ModifiableConst {
private:
    mutable int data1;  // Може модифікуватись навіть у const методах
    int data2;          // Не може модифікуватись у const методах без const_cast
    
public:
    ModifiableConst(int d1, int d2) : data1(d1), data2(d2) {}
    
    // Законний спосіб модифікації члена в const функції
    void updateData1() const {
        data1++;  // Дозволено, бо data1 є mutable
        std::cout << "data1 оновлено до " << data1 << " (через mutable)" << std::endl;
    }
    
    // Використання const_cast для модифікації не-mutable члена в const функції
    void updateData2() const {
        // Знімаємо константність з this
        ModifiableConst* nonConstThis = const_cast<ModifiableConst*>(this);
        
        // Тепер модифікуємо data2 через неконстантний вказівник
        nonConstThis->data2++;
        
        std::cout << "data2 оновлено до " << nonConstThis->data2 << " (через const_cast)" << std::endl;
    }
    
    void display() const {
        std::cout << "data1: " << data1 << ", data2: " << data2 << std::endl;
    }
};

int main() {
    ModifiableConst obj(10, 20);
    
    std::cout << "Початкові значення:" << std::endl;
    obj.display();
    
    std::cout << "\nМодифікація значень у const методах:" << std::endl;
    obj.updateData1();  // Використання mutable
    obj.updateData2();  // Використання const_cast
    
    std::cout << "\nФінальні значення:" << std::endl;
    obj.display();
    
    std::cout << "\nПримітка: Використання const_cast у const функції технічно можливе," << std::endl;
    std::cout << "але це порушує контракт константності і може призвести до невизначеної поведінки," << std::endl;
    std::cout << "якщо об'єкт був оголошений як const." << std::endl;
    
    // Демонстрація потенційної проблеми з const об'єктами
    std::cout << "\nДемонстрація з const об'єктом:" << std::endl;
    const ModifiableConst constObj(100, 200);
    constObj.display();
    
    constObj.updateData1();  // Безпечно, оскільки data1 є mutable
    constObj.updateData2();  // Потенційно небезпечно, результат непередбачуваний
    
    constObj.display();
    
    return 0;
}