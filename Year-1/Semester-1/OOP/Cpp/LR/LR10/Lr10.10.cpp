#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void copy_file() {
    ifstream in("input.txt", ios::binary);
    ofstream out("output.txt", ios::binary);
    char c;
    int count = 0;
    if (!in || !out) {
        cout << "File error\n";
        return;
    }
    while (in.get(c)) {
        out.put(c);
        count++;
    }
    cout << "Copied " << count << " characters\n";
}

void count_words() {
    ifstream in("input.txt");
    if (!in) {
        cout << "File error\n";
        return;
    }
    string word;
    int count = 0;
    char c;
    word.clear();
    while (in.get(c)) {
        if (isspace(c)) {
            if (word.length() >= 3) count++;
            word.clear();
        } else {
            word += c;
        }
    }
    if (word.length() >= 3) count++;
    cout << "Words with 3+ characters: " << count << '\n';
}

int main() {
    copy_file();
    count_words();
    return 0;
}