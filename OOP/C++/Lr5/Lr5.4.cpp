#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

// Custom exception class
class DivideByZeroException : public exception {
public:
    const char* what() const throw() {
        return "Division by zero error!";
    }
};

// Custom exception with additional information
class OutOfRangeException : public exception {
private:
    string message;
    
public:
    OutOfRangeException(int index, int max) {
        message = "Index " + to_string(index) + " out of range (max: " + to_string(max) + ")";
    }
    
    const char* what() const throw() {
        return message.c_str();
    }
};

// Class demonstrating built-in and custom exceptions
class SafeArray {
private:
    int* arr;
    int size;
    
public:
    // Constructor
    SafeArray(int s) : size(s) {
        if (s <= 0) {
            throw invalid_argument("Array size must be positive");
        }
        
        try {
            arr = new int[size];
            cout << "Array of size " << size << " created successfully" << endl;
        }
        catch (bad_alloc& e) {
            cout << "Memory allocation failed: " << e.what() << endl;
            throw; // Re-throw the exception
        }
        
        // Initialize array with zeros
        for (int i = 0; i < size; i++) {
            arr[i] = 0;
        }
    }
    
    // Destructor
    ~SafeArray() {
        cout << "SafeArray destructor called" << endl;
        delete[] arr;
    }
    
    // Subscript operator with bounds checking
    int& operator[](int index) {
        if (index < 0 || index >= size) {
            throw OutOfRangeException(index, size - 1);
        }
        return arr[index];
    }
    
    // Const version of subscript operator
    const int& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw OutOfRangeException(index, size - 1);
        }
        return arr[index];
    }
    
    // Get array size
    int getSize() const {
        return size;
    }
    
    // Display array
    void display() const {
        cout << "Array contents: ";
        for (int i = 0; i < size; i++) {
            cout << arr[i];
            if (i < size - 1) cout << ", ";
        }
        cout << endl;
    }
};

// Simple mathematical functions with exception handling
double safeDivide(double a, double b) {
    if (b == 0) {
        throw DivideByZeroException();
    }
    return a / b;
}

int main() {
    cout << "===== Exception Handling Demo =====" << endl;
    
    // 1. Handling built-in exceptions
    cout << "\n1. Handling standard exceptions:" << endl;
    try {
        cout << "Attempting to allocate a large array..." << endl;
        int* bigArray = new int[1000000000]; // May cause bad_alloc
        delete[] bigArray;
    }
    catch (bad_alloc& e) {
        cout << "Caught memory allocation exception: " << e.what() << endl;
    }
    
    // 2. Custom exception with the SafeArray class
    cout << "\n2. Working with SafeArray class:" << endl;
    try {
        // Create a safe array
        SafeArray numbers(5);
        
        // Set some values
        numbers[0] = 10;
        numbers[1] = 20;
        numbers[2] = 30;
        numbers[3] = 40;
        numbers[4] = 50;
        
        // Display array
        numbers.display();
        
        // Try to access an out-of-bounds element
        cout << "Attempting to access element at index 10..." << endl;
        numbers[10] = 100; // Should throw OutOfRangeException
    }
    catch (OutOfRangeException& e) {
        cout << "Caught array exception: " << e.what() << endl;
    }
    catch (exception& e) {
        cout << "Caught standard exception: " << e.what() << endl;
    }
    
    // 3. Invalid argument exception
    cout << "\n3. Invalid argument handling:" << endl;
    try {
        cout << "Attempting to create an array with negative size..." << endl;
        SafeArray badArray(-5); // Should throw invalid_argument
    }
    catch (invalid_argument& e) {
        cout << "Caught invalid argument exception: " << e.what() << endl;
    }
    
    // 4. Custom division exception
    cout << "\n4. Custom division exception:" << endl;
    try {
        double a = 10.0, b = 0.0;
        cout << "Attempting to divide " << a << " by " << b << "..." << endl;
        double result = safeDivide(a, b); // Should throw DivideByZeroException
        cout << "Result: " << result << endl; // This line won't execute
    }
    catch (DivideByZeroException& e) {
        cout << "Caught division exception: " << e.what() << endl;
    }
    
    // 5. Nested try-catch blocks
    cout << "\n5. Nested exception handling:" << endl;
    try {
        cout << "Outer try block" << endl;
        try {
            cout << "Inner try block" << endl;
            throw runtime_error("Exception from inner try block");
        }
        catch (invalid_argument& e) {
            cout << "Inner catch: " << e.what() << endl;
        }
        // The runtime_error will propagate to the outer catch
    }
    catch (runtime_error& e) {
        cout << "Outer catch: " << e.what() << endl;
    }
    
    // 6. Exception handling with resources
    cout << "\n6. Exception handling with resources:" << endl;
    int* resource = nullptr;
    try {
        cout << "Allocating resource..." << endl;
        resource = new int[100];
        
        cout << "Using resource..." << endl;
        // Simulate an exception while using the resource
        throw runtime_error("Error while using resource");
        
        // This won't execute due to the exception
        delete[] resource;
        resource = nullptr;
    }
    catch (exception& e) {
        cout << "Caught exception: " << e.what() << endl;
        
        // Clean up resources in the catch block
        if (resource != nullptr) {
            cout << "Cleaning up resource in catch block" << endl;
            delete[] resource;
            resource = nullptr;
        }
    }
    
    cout << "\nEnd of program" << endl;
    return 0;
}