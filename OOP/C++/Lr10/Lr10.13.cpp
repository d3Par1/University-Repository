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
    string line;
    while (getline(in, line)) {
        cout << line << '\n';
    }
    in.close();
    return 0;
}