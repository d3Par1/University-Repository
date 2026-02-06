#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string changestr(string str, bool P)
{
    // Розділяємо на 3 частини по 4 символи
    string part1 = str.substr(0, 4);
    string part2 = str.substr(4, 4);
    string part3 = str.substr(8, 4);

    if (P)
    {
        return part2 + part3 + part1; // 2-1-3
    }
    else
    {
        return part3 + part1 + part2; // 3-1-2
    }
}

int main()
{
    string word;
    bool P;

    cout << "Введіть слово з 12 літер: ";
    cin >> word;

    cout << "Введіть P (1 або 0): ";
    cin >> P;

    string result = changestr(word, P);

    cout << "Було: " << word << endl;
    cout << "Стало: " << result << endl;

    // Збереження у файл
    ofstream file("result.txt");
    file << "Було: " << word << endl;
    file << "Стало: " << result << endl;
    file.close();

    return 0;
}