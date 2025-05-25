#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ofstream out("log.txt");
    if (!out) {
        cout << "File error\n";
        return 1;
    }
    out << "Critical data";
    out.flush(); // Ensure data is written immediately
    cout << "flush() is useful in:\n"
         << "1. Logging critical data (e.g., crash diagnostics).\n"
         << "2. Real-time applications needing immediate output.\n"
         << "3. Debugging to ensure output is not lost.\n"
         << "4. Systems with frequent power interruptions.\n";
    out.close();
    return 0;
}