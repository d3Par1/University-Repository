#include <iostream>
#include <cstring>
using namespace std;

class MyString {
private:
    char* str;     // Pointer to string
    int length;    // Length of string
public:
    // Constructor
    MyString(const char* s = "") {
        length = strlen(s);
        str = new char[length + 1]; // +1 for null terminator
        strcpy(str, s);
        cout << "Constructor called for: " << str << endl;
    }
    
    // Copy constructor
    MyString(const MyString& source) {
        length = source.length;
        str = new char[length + 1];
        strcpy(str, source.str);
        cout << "Copy constructor called for: " << str << endl;
    }
    
    // Destructor
    ~MyString() {
        cout << "Destructor called for: " << str << endl;
        delete[] str;
    }
    
    // Assignment operator
    MyString& operator=(const MyString& right) {
        if (this == &right) {
            return *this; // Handle self-assignment
        }
        
        // Free old memory
        delete[] str;
        
        // Allocate new memory and copy content
        length = right.length;
        str = new char[length + 1];
        strcpy(str, right.str);
        cout << "Assignment operator called for: " << str << endl;
        
        return *this;
    }
    
    // Get string length
    int getLength() const {
        return length;
    }
    
    // Get string content
    const char* getString() const {
        return str;
    }
    
    // Concatenate two strings and return a new string
    MyString operator+(const MyString& right) const {
        // Create a new string with enough space for both strings
        char* temp = new char[length + right.length + 1];
        strcpy(temp, str);
        strcat(temp, right.str);
        
        // Create new MyString object with combined string
        MyString result(temp);
        
        // Clean up temporary memory
        delete[] temp;
        
        return result;
    }
    
    // Display string
    void display() const {
        cout << "String: " << str << ", Length: " << length << endl;
    }
};

int main() {
    // Create string objects
    MyString s1("Hello");
    MyString s2("World");
    
    // Display strings
    cout << "\nString details:" << endl;
    s1.display();
    s2.display();
    
    // Test copy constructor
    cout << "\nTesting copy constructor:" << endl;
    MyString s3 = s1; // Copy constructor called
    s3.display();
    
    // Test assignment operator
    cout << "\nTesting assignment operator:" << endl;
    MyString s4;
    s4 = s2; // Assignment operator called
    s4.display();
    
    // Test concatenation
    cout << "\nTesting concatenation:" << endl;
    MyString s5 = s1 + s2;
    s5.display();
    
    // Test multiple operations
    cout << "\nTesting multiple operations:" << endl;
    MyString s6;
    s6 = s1 + MyString(" ") + s2;
    s6.display();
    
    cout << "\nEnd of program" << endl;
    return 0;
}