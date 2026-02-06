#include <iostream>
#include <string>

// Приклад простору імен для демонстрації концепції
namespace my_library {
    void print(const std::string& msg) {
        std::cout << "my_library print: " << msg << std::endl;
    }
    
    int max(int a, int b) {
        return (a > b) ? a : b;
    }
}

// Використання глобального простору імен для демонстрації
void print(const std::string& msg) {
    std::cout << "Глобальний print: " << msg << std::endl;
}

int max(int a, int b) {
    return (a < b) ? a : b; // Навмисно протилежна семантика для демонстрації
}

int main() {
    std::cout << "Переваги розміщення бібліотеки в просторі імен std:" << std::endl;
    std::cout << "1. Уникнення конфліктів імен з кодом користувача" << std::endl;
    std::cout << "2. Можливість вибіркового імпорту з використанням 'using'" << std::endl;
    std::cout << "3. Забезпечення організації та структури бібліотеки" << std::endl;
    std::cout << "4. Забезпечення сумісності з існуючим кодом" << std::endl;
    std::cout << "5. Полегшення розширення бібліотеки в майбутньому" << std::endl;
    
    // Демонстрація переваг простору імен
    std::cout << "\nДемонстрація:" << std::endl;
    
    // Використання повних кваліфікованих імен для уникнення двозначності
    std::string message = "Привіт, Світ!";
    std::cout << "Використання std::cout: " << message << std::endl;
    
    // Демонстрація конфлікту імен функцій
    print("Виклик глобального print");
    my_library::print("Виклик print з простору імен");
    
    // Демонстрація відмінностей в функціях з однаковим ім'ям
    int a = 5, b = 10;
    std::cout << "\nmax(" << a << ", " << b << ") в глобальному просторі: " << max(a, b) << std::endl;
    std::cout << "max(" << a << ", " << b << ") в my_library: " << my_library::max(a, b) << std::endl;
    
    // Вибірковий using
    std::cout << "\nВикористання вибіркового using:" << std::endl;
    using std::cout;
    cout << "Використання лише std::cout (без std::)" << std::endl;
    
    // Без простору імен std, наступні конфлікти могли б виникнути:
    std::cout << "\nПотенційні проблеми без простору імен std:" << std::endl;
    std::cout << "- Конфлікт з власними функціями cout, cin, endl тощо" << std::endl;
    std::cout << "- Неможливість створити свою версію елементів бібліотеки" << std::endl;
    std::cout << "- Забруднення глобального простору імен" << std::endl;
    
    return 0;
}