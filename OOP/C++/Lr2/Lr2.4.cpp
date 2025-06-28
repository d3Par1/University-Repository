#include <iostream>
#include <string>

using namespace std;

class card {
private:
    string bookTitle;
    string authorName;
    int copiesIssued;
    
public:
    void store(string title, string author, int copies) {
        bookTitle = title;
        authorName = author;
        copiesIssued = copies;
    }
    
    void show() {
        cout << "Назва книги: " << bookTitle << endl;
        cout << "Автор: " << authorName << endl;
        cout << "Кількість виданих примірників: " << copiesIssued << endl;
        cout << "-------------------------" << endl;
    }
};

int main() {
    card book1, book2, book3;
    
    // Зберігаємо інформацію про книги
    book1.store("1984", "Джордж Орвелл", 5);
    book2.store("Гаррі Поттер", "Дж.К. Роулінг", 10);
    book3.store("Майстер і Маргарита", "Михайло Булгаков", 3);
    
    // Виводимо інформацію
    cout << "Бібліотечний каталог:" << endl;
    cout << "-------------------------" << endl;
    book1.show();
    book2.show();
    book3.show();
    
    return 0;
}
// This code defines a simple library card system in C++.