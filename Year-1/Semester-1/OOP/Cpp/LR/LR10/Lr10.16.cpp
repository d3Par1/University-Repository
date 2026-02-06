#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    string input;
    cout << "Enter text: ";
    getline(cin, input);
    ofstream out("input.txt");
    out << input;
    out.close();
    ifstream in("input.txt");
    string content;
    char c;
    while (in.get(c)) content += c;
    in.close();
    string result;
    for (size_t i = 0; i < content.size(); i += 2) {
        if (i + 1 < content.size()) {
            result += content[i + 1];
            result += content[i];
        } else {
            result += content[i];
        }
    }
    out.open("output.txt");
    out << result;
    out.close();
    cout << "Swapped: " << result << '\n';
    return 0;
}