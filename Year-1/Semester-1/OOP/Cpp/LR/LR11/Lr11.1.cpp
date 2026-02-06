#include <iostream>
#include <iomanip>

std::ostream& upperE(std::ostream& os) {
    os.setf(std::ios::uppercase | std::ios::scientific);
    return os;
}

int main() {
    double numbers[] = {0.000123, 123456.789, 0.0, 1.0, -3.14159};
    
    std::cout << "Звичайне виведення чисел:" << std::endl;
    for (double num : numbers) {
        std::cout << std::scientific << num << std::endl;
    }
    
    std::cout << "\nВиведення з використанням власного маніпулятора (символ E в верхньому регістрі):" << std::endl;
    for (double num : numbers) {
        std::cout << upperE << num << std::endl;
    }
    
    return 0;
}