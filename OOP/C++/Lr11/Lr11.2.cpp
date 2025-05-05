#include <iostream>
#include <fstream>
#include <string>

#define TAB_SIZE 4 

int main() {
    std::string inputFileName = "input.txt";
    std::string outputFileName = "output.txt";
    
    std::ifstream inFile(inputFileName);
    if (!inFile) {
        std::cerr << "Помилка відкриття файлу " << inputFileName << std::endl;
        return 1;
    }
    
    std::ofstream outFile(outputFileName);
    if (!outFile) {
        std::cerr << "Помилка створення файлу " << outputFileName << std::endl;
        inFile.close();
        return 1;
    }
    
    char ch;
    while (inFile.get(ch)) {
        if (ch == '\t') {
            for (int i = 0; i < TAB_SIZE; i++) {
                outFile.put(' ');
            }
        } else {
            outFile.put(ch);
        }
    }
    
    inFile.close();
    outFile.close();
    
    inFile.open(inputFileName);
    if (!inFile) {
        std::cerr << "Помилка повторного відкриття файлу " << inputFileName << std::endl;
        return 1;
    }
    
    std::cout << "Вміст файлу з заміною пробілів та табуляцій:" << std::endl;
    
    std::string line;
    while (std::getline(inFile, line)) {
        for (size_t i = 0; i < line.length(); ++i) {
            if (line[i] == ' ') {
                std::cout << "1";
            } else if (line[i] == '\t') {
                int tabCount = 1;
                while (i + 1 < line.length() && line[i + 1] == '\t') {
                    tabCount++;
                    i++;
                } 
                std::cout << (tabCount * TAB_SIZE);
            } else {
                std::cout << line[i];
            }
        }
        std::cout << std::endl;
    }
    
    inFile.close();
    
    std::cout << "\nФайл успішно скопійовано з " << inputFileName << " до " << outputFileName << std::endl;
    std::cout << "У файлі " << outputFileName << " всі символи табуляції замінено на " << TAB_SIZE << " пробіли." << std::endl;
    
    return 0;
}