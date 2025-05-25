#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ifstream in("input.txt");
    ofstream out("output.txt");
    char c;
    int count = 0;
    if (!in || !out) {
        cout << "File error\n";
        return 1;
    }
    while (in.get(c)) {
        out.put(c);
        count++;
    }
    in.close();
    out.close();
    cout << "Copied " << count << " characters\n";
    cout << "Difference in file size may occur due to line ending conversions (e.g., CR+LF vs LF).\n";
    return 0;
}