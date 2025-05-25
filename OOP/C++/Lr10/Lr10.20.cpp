#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

int main() {
    ifstream in("input.txt");
    if (!in) {
        cout << "File error\n";
        return 1;
    }
    int freq[26] = {0};
    char c;
    while (in.get(c)) {
        if (isalpha(c)) {
            c = tolower(c);
            freq[c - 'a']++;
        }
    }
    in.close();
    cout << "Vowels:\n";
    for (char c : {'a', 'e', 'i', 'o', 'u'}) {
        cout << c << ": " << freq[c - 'a'] << '\n';
    }
    cout << "Consonants:\n";
    for (char c = 'a'; c <= 'z'; c++) {
        if (c != 'a' && c != 'e' && c != 'i' && c != 'o' && c != 'u') {
            cout << c << ": " << freq[c - 'a'] << '\n';
        }
    }
    return 0;
}