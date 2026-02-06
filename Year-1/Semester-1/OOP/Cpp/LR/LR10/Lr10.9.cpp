#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    ifstream in("input.txt");
    if (!in) {
        cout << "File error\n";
        return 1;
    }
    string word;
    int count = 0;
    while (in >> word) {
        if (word.length() >= 3) count++;
    }
    in.close();
    cout << "Words with 3+ characters: " << count << '\n';
    return 0;
}