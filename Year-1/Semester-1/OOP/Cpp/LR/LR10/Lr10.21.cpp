#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#define SIZE 40

class inventory {
    char item[SIZE];
    int onhand;
    double cost;
public:
    inventory(char *i, int o, double c) {
        strcpy(item, i);
        onhand = o;
        cost = c;
    }
    void store(fstream &stream) {
        stream.write((char*)this, sizeof(*this));
    }
    void retrieve(fstream &stream) {
        stream.read((char*)this, sizeof(*this));
    }
    friend ostream &operator<<(ostream &stream, inventory obj);
    friend istream &operator>>(istream &stream, inventory &obj);
};

ostream &operator<<(ostream &stream, inventory obj) {
    stream << obj.item << ": " << obj.onhand << " on hand at $" << obj.cost << '\n';
    return stream;
}

istream &operator>>(istream &stream, inventory &obj) {
    cout << "Enter item name: ";
    stream.getline(obj.item, SIZE);
    cout << "Enter number on hand: ";
    stream >> obj.onhand;
    cout << "Enter cost: ";
    stream >> obj.cost;
    stream.ignore();
    return stream;
}

int main() {
    fstream file("inventory.dat", ios::binary | ios::in | ios::out | ios::trunc);
    if (!file) {
        cout << "File error\n";
        return 1;
    }
    inventory items[] = {
        inventory("Item1", 10, 5.99),
        inventory("Item2", 20, 9.99),
        inventory("Item3", 30, 14.99)
    };
    for (int i = 0; i < 3; i++) {
        items[i].store(file);
    }
    int record;
    cout << "Enter record number (0-2): ";
    cin >> record;
    file.seekg(record * sizeof(inventory), ios::beg);
    inventory item("", 0, 0);
    item.retrieve(file);
    cout << item;
    file.close();
    return 0;
}