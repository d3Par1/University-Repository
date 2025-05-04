#include <iostream>
#include <iomanip>

std::ostream& scientific_upper(std::ostream& os) {
    return os << std::scientific << std::uppercase;
}

int main() {
    double num = 12345.6789;
    std::cout << scientific_upper << num << std::endl;
    return 0;
}