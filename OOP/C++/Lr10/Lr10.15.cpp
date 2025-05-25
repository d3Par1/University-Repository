#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void reverse_full() {
    ifstream in("input.txt");
    if (!in) {
        cout << "File error\n";
        return;
    }
    string content;
    char c;
    while (in.get(c)) content += c;
    in.close();
    cout << "Full reverse:\n";
    for (int i = content.size() - 1; i >= 0; i--) cout << content[i];
    cout << '\n';
}

void reverse_lines() {
    ifstream in("input.txt");
    if (!in) {
        cout << "File error\n";
        return;
    }
    string line;
    cout << "Line-by-line reverse:\n";
    while (getline(in, line)) {
        for (int i = line.size() - 1; i >= 0; i--) cout << line[i];
        cout << '\n';
    }
    in.close();
}

int main() {
    reverse_full();
    reverse_lines();
    return 0;
}