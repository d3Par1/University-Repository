#include <cstring>
#include <iostream>
using namespace std;

void center(char *s) {
    int len = strlen(s);
    int spaces = (80 - len) / 2;
    cout.width(spaces + len);
    cout << s << '\n';
}

int main() {
    center("Hello");
    center("Centered Text");
    return 0;
}