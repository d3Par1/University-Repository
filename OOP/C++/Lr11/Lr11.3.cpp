#include <iostream>
#include <fstream>
#include <cstring>

bool compareWords(const char* word1, const char* word2) {
    int i = 0;
    while (word1[i] != '\0' && word2[i] != '\0') {
        if (tolower(word1[i]) != tolower(word2[i])) {
            return false;
        }
        i++;
    }
    return (word1[i] == '\0' && word2[i] == '\0');
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Використання: " << argv[0] << " <назва_файлу> <слово_для_пошуку>" << std::endl;
        return 1;
    }
    
    const char* filename = argv[1];
    const char* searchWord = argv[2];
    
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Помилка відкриття файлу " << filename << std::endl;
        return 1;
    }
    
    int wordCount = 0;
    char buffer[1024]; 
    char word[256] = "";
    int wordLength = 0;
    
    while (file.get(buffer[0])) {
        if (buffer[0] == ' ' || buffer[0] == '\n' || buffer[0] == '\t' || buffer[0] == '\r') {
            if (wordLength > 0) {
                word[wordLength] = '\0'; 
                if (compareWords(word, searchWord)) {
                    wordCount++;
                }
                wordLength = 0; 
            }
        } else {
            if (wordLength < 255) { 
                word[wordLength++] = buffer[0];
            }
        }
    }
    
    if (wordLength > 0) {
        word[wordLength] = '\0';
        if (compareWords(word, searchWord)) {
            wordCount++;
        }
    }
    
    file.close();
    
    std::cout << "Слово \"" << searchWord << "\" знайдено у файлі " << wordCount << " разів." << std::endl;
    
    return 0;
}