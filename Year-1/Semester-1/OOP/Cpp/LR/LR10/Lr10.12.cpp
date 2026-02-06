#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

class account {
    int custnum;
    char name[80];
    double balance;
public:
    account(int c, char *n, double b) : custnum(c), balance(b) {
        strcpy(name, n);
    }
    friend ostream &operator<<(ostream &stream, account obj);
};

ostream &operator<<(ostream &stream, account obj) {
    stream << obj.custnum << ", " << obj.name << ", " << obj.balance;
    return stream;
}

int main() {
    account a(1, "John Doe", 1000.50);
    ofstream out("accounts.txt");
    if (!out) {
        cout << "File error\n";
        return 1;
    }
    out << a << '\n';
    out.close();
    cout << "No getline used in Task 11, so behavior is unchanged.\n";
    return 0;
}