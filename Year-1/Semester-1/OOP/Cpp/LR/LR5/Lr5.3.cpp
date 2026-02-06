#include <iostream>
using namespace std;

// Template Stack class
template <class T, int size = 10>
class Stack {
private:
    T* items;      // Dynamic array to store stack items
    int top;       // Index of top element
    int capacity;  // Maximum stack capacity
    
public:
    // Constructor
    Stack() : top(-1), capacity(size) {
        items = new T[capacity];
        cout << "Stack constructor: capacity = " << capacity << endl;
    }
    
    // Destructor
    ~Stack() {
        cout << "Stack destructor called" << endl;
        delete[] items;
    }
    
    // Copy constructor
    Stack(const Stack& other) : top(other.top), capacity(other.capacity) {
        cout << "Stack copy constructor called" << endl;
        items = new T[capacity];
        for (int i = 0; i <= top; i++) {
            items[i] = other.items[i];
        }
    }
    
    // Assignment operator
    Stack& operator=(const Stack& other) {
        cout << "Stack assignment operator called" << endl;
        if (this != &other) {
            delete[] items;
            top = other.top;
            capacity = other.capacity;
            items = new T[capacity];
            for (int i = 0; i <= top; i++) {
                items[i] = other.items[i];
            }
        }
        return *this;
    }
    
    // Push item onto stack
    void push(const T& item) {
        if (isFull()) {
            cout << "Stack overflow!" << endl;
            return;
        }
        items[++top] = item;
    }
    
    // Pop item from stack
    T pop() {
        if (isEmpty()) {
            cout << "Stack underflow!" << endl;
            // Return default value in case of error
            return T();
        }
        return items[top--];
    }
    
    // Peek at top item without removing
    T peek() const {
        if (isEmpty()) {
            cout << "Stack is empty!" << endl;
            // Return default value in case of error
            return T();
        }
        return items[top];
    }
    
    // Check if stack is empty
    bool isEmpty() const {
        return top == -1;
    }
    
    // Check if stack is full
    bool isFull() const {
        return top == capacity - 1;
    }
    
    // Get current stack size
    int getSize() const {
        return top + 1;
    }
    
    // Get stack capacity
    int getCapacity() const {
        return capacity;
    }
    
    // Display stack contents
    void display() const {
        if (isEmpty()) {
            cout << "Stack is empty" << endl;
            return;
        }
        
        cout << "Stack contents (top to bottom): ";
        for (int i = top; i >= 0; i--) {
            cout << items[i];
            if (i > 0) cout << ", ";
        }
        cout << endl;
    }
};

// Example class to use with the Stack template
class Person {
private:
    string name;
    int age;
    
public:
    // Default constructor
    Person() : name("Unknown"), age(0) {}
    
    // Parameterized constructor
    Person(string n, int a) : name(n), age(a) {}
    
    // Getters
    string getName() const { return name; }
    int getAge() const { return age; }
    
    // Overload insertion operator for easy output
    friend ostream& operator<<(ostream& os, const Person& p) {
        os << p.name << " (" << p.age << ")";
        return os;
    }
};

int main() {
    cout << "===== Integer Stack Demo =====" << endl;
    Stack<int, 5> intStack;
    
    cout << "\nPushing integers onto stack:" << endl;
    for (int i = 1; i <= 6; i++) {
        cout << "Pushing " << i << ": ";
        intStack.push(i);
        intStack.display();
    }
    
    cout << "\nPopping from integer stack:" << endl;
    while (!intStack.isEmpty()) {
        cout << "Popped " << intStack.pop() << ": ";
        intStack.display();
    }
    
    cout << "\n===== Double Stack Demo =====" << endl;
    Stack<double> doubleStack;
    
    cout << "\nPushing doubles onto stack:" << endl;
    doubleStack.push(1.1);
    doubleStack.push(2.2);
    doubleStack.push(3.3);
    doubleStack.display();
    
    cout << "\n===== String Stack Demo =====" << endl;
    Stack<string, 3> stringStack;
    
    cout << "\nPushing strings onto stack:" << endl;
    stringStack.push("Hello");
    stringStack.push("World");
    stringStack.push("C++");
    stringStack.display();
    
    cout << "\n===== Person Stack Demo =====" << endl;
    Stack<Person, 3> personStack;
    
    cout << "\nPushing Person objects onto stack:" << endl;
    personStack.push(Person("Alice", 25));
    personStack.push(Person("Bob", 30));
    personStack.push(Person("Charlie", 35));
    personStack.display();
    
    cout << "\nTesting copy constructor:" << endl;
    Stack<int, 5> intStackCopy = intStack;
    cout << "Original stack: ";
    intStack.display();
    cout << "Copied stack: ";
    intStackCopy.display();
    
    cout << "\nEnd of program" << endl;
    return 0;
}