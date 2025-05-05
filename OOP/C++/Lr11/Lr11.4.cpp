#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string filename = "test_file.txt";
    
    // Створюємо тестовий файл з достатньою кількістю байтів
    std::ofstream writeFile(filename);
    if (!writeFile) {
        std::cerr << "Помилка створення файлу " << filename << std::endl;
        return 1;
    }
    
    // Заповнюємо файл даними (більше 81 байту)
    for (int i = 0; i < 100; i++) {
        writeFile << "Byte " << i << ": This is test data for position demonstration.\n";
    }
    writeFile.close();
    
    // Відкриваємо файл для запису та встановлюємо вказівник на 81-й байт
    std::fstream out(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!out) {
        std::cerr << "Помилка відкриття файлу " << filename << std::endl;
        return 1;
    }
    
    // Виводимо поточну позицію
    std::cout << "Початкова позиція: " << out.tellp() << std::endl;
    
    // Встановлюємо вказівник запису на 81-й байт
    out.seekp(80, std::ios::beg);  // 81-й байт (починаючи з 0)
    
    // Виводимо нову позицію для перевірки
    std::cout << "Після seekp(80): " << out.tellp() << std::endl;
    
    // Демонстрація, що вказівник дійсно встановлено правильно
    // Записуємо маркер та змінюємо дані на цій позиції
    out << "**МАРКЕР ПОЗИЦІЇ 81**";
    
    out.close();
    
    // Перевіряємо результат
    std::ifstream readFile(filename);
    if (!readFile) {
        std::cerr << "Помилка відкриття файлу для перевірки " << filename << std::endl;
        return 1;
    }
    
    char buffer[100];
    readFile.seekg(60, std::ios::beg);  // Починаємо трохи раніше, щоб побачити контекст
    readFile.read(buffer, 100);
    buffer[99] = '\0';  // Гарантуємо, що рядок закінчується нульовим символом
    
    std::cout << "\nВміст файлу навколо позиції 81 (починаючи з позиції 60):\n" << buffer << std::endl;
    
    readFile.close();
    
    std::cout << "\nІнструкція, яка встановлює вказівник запису на 81-й байт:" << std::endl;
    std::cout << "out.seekp(80, std::ios::beg);" << std::endl;
    
    return 0;
}