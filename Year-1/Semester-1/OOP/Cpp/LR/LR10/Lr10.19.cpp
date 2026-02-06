#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

void copy_and_swap() {
    ifstream in("input.txt");
    ofstream out("output.txt");
    if (!in || !out) {
        cout << "File error\n";
        return;
    }
    char c;
    while (in.get(c)) {
        if (isalpha(c)) {
            if (isupper(c)) out.put(tolower(c));
            else out.put(toupper(c));
        } else {
            out.put(c);
        }
    }
    in.close();
    out.close();
}

void display_centered() {
    ifstream in("output.txt");
    if (!in) {
        cout << "File error\n";
        return;
    }
    string line;
    while (getline(in, line)) {
        int len = line.length();
        int spaces = (80 - len) / 2;
        cout.fill('_');
        cout.width(spaces + len);
        cout << line << '\n';
    }
    in.close();
}

int main() {
    copy_and_swap();
    display_centered();
    return 0;
}