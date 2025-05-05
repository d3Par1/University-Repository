#include <iostream>
#include <fstream>
#include <string>

#define TAB_SIZE 4

int main() {
    const std::string inputFileName = "input.txt";
    const std::string outputFileName = "output.txt";
    
    // Open input file
    std::ifstream inFile(inputFileName);
    if (!inFile) {
        std::cerr << "Error opening file " << inputFileName << std::endl;
        return 1;
    }
    
    // Open output file
    std::ofstream outFile(outputFileName);
    if (!outFile) {
        std::cerr << "Error creating file " << outputFileName << std::endl;
        inFile.close();
        return 1;
    }
    
    // Replace tabs with spaces in the output file
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
    
    // Reopen input file to display content with space/tab visualization
    inFile.open(inputFileName);
    if (!inFile) {
        std::cerr << "Error reopening file " << inputFileName << std::endl;
        return 1;
    }
    
    std::cout << "File content with space and tab replacement:" << std::endl;
    
    std::string line;
    while (std::getline(inFile, line)) {
        for (size_t i = 0; i < line.length(); ++i) {
            if (line[i] == ' ') {
                std::cout << "1";  // Replace spaces with '1'
            } else if (line[i] == '\t') {
                int tabCount = 1;
                while (i + 1 < line.length() && line[i + 1] == '\t') {
                    tabCount++;
                    i++;
                }
                
                std::cout << (tabCount * TAB_SIZE);  // Show tab size
            } else {
                std::cout << line[i];  // Print other characters normally
            }
        }
        std::cout << std::endl;
    }
    
    inFile.close();
    
    std::cout << "\nFile successfully copied from " << inputFileName << " to " << outputFileName << std::endl;
    std::cout << "In file " << outputFileName << " all tab characters have been replaced with " << TAB_SIZE << " spaces." << std::endl;
    
    return 0;
}