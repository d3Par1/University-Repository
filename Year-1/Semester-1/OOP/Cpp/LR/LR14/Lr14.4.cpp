#include <iostream>
#include <string>

class OutputDevice {
    std::string name;
    bool bufferAvailable;
public:
    OutputDevice(const std::string& n, bool bufAvail = true) 
        : name(n), bufferAvailable(bufAvail) {}
    
    bool isBufferAvailable() const {
        return bufferAvailable;
    }
    
    void setBufferState(bool state) {
        bufferAvailable = state;
    }
    
    std::string getName() const {
        return name;
    }
    
    void outputChar(char ch) {
        if (bufferAvailable) {
            std::cout << "Пристрій \"" << name << "\" виводить: " << ch << std::endl;
        } else {
            std::cout << "Виведення на пристрій \"" << name << "\" заборонено - буфер зайнятий!" << std::endl;
        }
    }
};

int main() {
    // Створюємо три пристрої виведення
    OutputDevice dev1("Консоль", true);
    OutputDevice dev2("Принтер", false);
    OutputDevice dev3("Файл", true);
    
    // Спроба виводу на всі пристрої
    dev1.outputChar('A');
    dev2.outputChar('B');
    dev3.outputChar('C');
    
    // Відображення статусу пристроїв
    std::cout << "\nСтатус пристроїв виведення:" << std::endl;
    std::cout << dev1.getName() << ": " 
              << (dev1.isBufferAvailable() ? "Дозволено" : "Заборонено") << std::endl;
    std::cout << dev2.getName() << ": " 
              << (dev2.isBufferAvailable() ? "Дозволено" : "Заборонено") << std::endl;
    std::cout << dev3.getName() << ": " 
              << (dev3.isBufferAvailable() ? "Дозволено" : "Заборонено") << std::endl;
    
    return 0;
}