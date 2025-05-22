#include <iostream>
using namespace std;

class Complex {
private:
    double real;
    double imag;
public:
    // Constructor
    Complex(double r = 0, double i = 0) : real(r), imag(i) {
        cout << "Constructor called: " << real << " + " << imag << "i" << endl;
    }
    
    // Destructor
    ~Complex() {
        cout << "Destructor called: " << real << " + " << imag << "i" << endl;
    }
    
    // Copy constructor
    Complex(const Complex& other) : real(other.real), imag(other.imag) {
        cout << "Copy constructor called: " << real << " + " << imag << "i" << endl;
    }
    
    // Assignment operator
    Complex& operator=(const Complex& other) {
        cout << "Assignment operator called" << endl;
        if (this != &other) {
            real = other.real;
            imag = other.imag;
        }
        return *this;
    }
    
    // Addition operator
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    
    // Subtraction operator
    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    
    // Multiplication operator
    Complex operator*(const Complex& other) const {
        // (a+bi) * (c+di) = (ac-bd) + (ad+bc)i
        double newReal = real * other.real - imag * other.imag;
        double newImag = real * other.imag + imag * other.real;
        return Complex(newReal, newImag);
    }
    
    // Unary minus operator
    Complex operator-() const {
        return Complex(-real, -imag);
    }
    
    // Equality operator
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }
    
    // Inequality operator
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }
    
    // Friend function for output stream
    friend ostream& operator<<(ostream& os, const Complex& c);
    
    // Friend function for input stream
    friend istream& operator>>(istream& is, Complex& c);
};

// Output stream operator implementation
ostream& operator<<(ostream& os, const Complex& c) {
    if (c.imag >= 0)
        os << c.real << " + " << c.imag << "i";
    else
        os << c.real << " - " << -c.imag << "i";
    return os;
}

// Input stream operator implementation
istream& operator>>(istream& is, Complex& c) {
    cout << "Enter real part: ";
    is >> c.real;
    cout << "Enter imaginary part: ";
    is >> c.imag;
    return is;
}

int main() {
    // Create complex numbers
    Complex a(3, 4);
    Complex b(1, 2);
    
    // Test operator overloading
    cout << "\nComplex number a: " << a << endl;
    cout << "Complex number b: " << b << endl;
    
    Complex c = a + b;
    cout << "a + b = " << c << endl;
    
    Complex d = a - b;
    cout << "a - b = " << d << endl;
    
    Complex e = a * b;
    cout << "a * b = " << e << endl;
    
    Complex f = -a;
    cout << "-a = " << f << endl;
    
    // Test comparison operators
    cout << "\nTesting comparison operators:" << endl;
    Complex g(3, 4);
    cout << "a == g: " << (a == g) << endl;
    cout << "a != b: " << (a != b) << endl;
    
    // Test input
    cout << "\nEnter a new complex number:" << endl;
    Complex h;
    cin >> h;
    cout << "You entered: " << h << endl;
    
    cout << "\nEnd of program" << endl;
    return 0;
}