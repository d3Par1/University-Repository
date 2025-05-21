#include <iostream>
#include <cmath>

class Pwr {
    int base; 
    int exp;
public:
    Pwr(int b, int e) { base = b; exp = e; }
    
    // Функція перетворення в ціле число
    operator int() const {
        return static_cast<int>(pow(base, exp));
    }
};

int main() {
    Pwr p1(2, 3);  // 2^3 = 8
    Pwr p2(5, 2);  // 5^2 = 25
    
    // Неявне перетворення
    int result1 = p1;
    int result2 = p2;
    
    std::cout << "2^3 = " << result1 << std::endl;
    std::cout << "5^2 = " << result2 << std::endl;
    
    // Явне перетворення
    std::cout << "3^4 = " << static_cast<int>(Pwr(3, 4)) << std::endl;
    
    return 0;
}