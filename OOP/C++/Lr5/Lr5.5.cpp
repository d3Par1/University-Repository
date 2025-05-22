#include <iostream>
#include <string>
#include <cmath>
using namespace std;

// Base class: Shape
class Shape {
protected:
    string name;
    string color;
    
public:
    // Constructor
    Shape(const string& n = "Unknown", const string& c = "White") 
        : name(n), color(c) {
        cout << "Shape constructor called: " << name << endl;
    }
    
    // Destructor
    virtual ~Shape() {
        cout << "Shape destructor called: " << name << endl;
    }
    
    // Pure virtual function for calculating area
    virtual double area() const = 0;
    
    // Pure virtual function for calculating perimeter
    virtual double perimeter() const = 0;
    
    // Virtual function for displaying information
    virtual void display() const {
        cout << "Shape: " << name << ", Color: " << color << endl;
    }
    
    // Getters and setters
    string getName() const { return name; }
    void setName(const string& n) { name = n; }
    
    string getColor() const { return color; }
    void setColor(const string& c) { color = c; }
};

// Derived class: Circle
class Circle : public Shape {
private:
    double radius;
    
public:
    // Constructor
    Circle(double r = 1.0, const string& c = "White") 
        : Shape("Circle", c), radius(r) {
        cout << "Circle constructor called, radius = " << radius << endl;
    }
    
    // Destructor
    ~Circle() {
        cout << "Circle destructor called, radius = " << radius << endl;
    }
    
    // Calculate area
    double area() const override {
        return 3.14159 * radius * radius;
    }
    
    // Calculate perimeter (circumference)
    double perimeter() const override {
        return 2 * 3.14159 * radius;
    }
    
    // Display information
    void display() const override {
        Shape::display();
        cout << "Radius: " << radius << endl;
        cout << "Area: " << area() << endl;
        cout << "Perimeter: " << perimeter() << endl;
    }
    
    // Getters and setters
    double getRadius() const { return radius; }
    void setRadius(double r) { radius = r; }
};

// Derived class: Rectangle
class Rectangle : public Shape {
private:
    double width;
    double height;
    
public:
    // Constructor
    Rectangle(double w = 1.0, double h = 1.0, const string& c = "White") 
        : Shape("Rectangle", c), width(w), height(h) {
        cout << "Rectangle constructor called, width = " << width 
             << ", height = " << height << endl;
    }
    
    // Destructor
    ~Rectangle() {
        cout << "Rectangle destructor called, width = " << width 
             << ", height = " << height << endl;
    }
    
    // Calculate area
    double area() const override {
        return width * height;
    }
    
    // Calculate perimeter
    double perimeter() const override {
        return 2 * (width + height);
    }
    
    // Display information
    void display() const override {
        Shape::display();
        cout << "Width: " << width << ", Height: " << height << endl;
        cout << "Area: " << area() << endl;
        cout << "Perimeter: " << perimeter() << endl;
    }
    
    // Getters and setters
    double getWidth() const { return width; }
    void setWidth(double w) { width = w; }
    
    double getHeight() const { return height; }
    void setHeight(double h) { height = h; }
};

// Derived class: Triangle
class Triangle : public Shape {
private:
    double side1, side2, side3;
    
public:
    // Constructor
    Triangle(double s1 = 1.0, double s2 = 1.0, double s3 = 1.0, const string& c = "White") 
        : Shape("Triangle", c), side1(s1), side2(s2), side3(s3) {
        cout << "Triangle constructor called, sides = " << side1 << ", " 
             << side2 << ", " << side3 << endl;
    }
    
    // Destructor
    ~Triangle() {
        cout << "Triangle destructor called" << endl;
    }
    
    // Calculate area using Heron's formula
    double area() const override {
        double s = (side1 + side2 + side3) / 2;
        return sqrt(s * (s - side1) * (s - side2) * (s - side3));
    }
    
    // Calculate perimeter
    double perimeter() const override {
        return side1 + side2 + side3;
    }
    
    // Display information
    void display() const override {
        Shape::display();
        cout << "Sides: " << side1 << ", " << side2 << ", " << side3 << endl;
        cout << "Area: " << area() << endl;
        cout << "Perimeter: " << perimeter() << endl;
    }
    
    // Getters and setters
    double getSide1() const { return side1; }
    void setSide1(double s) { side1 = s; }
    
    double getSide2() const { return side2; }
    void setSide2(double s) { side2 = s; }
    
    double getSide3() const { return side3; }
    void setSide3(double s) { side3 = s; }
};

// Function to display shape information using base class pointer
void displayShapeInfo(const Shape* shape) {
    cout << "\nShape Information:" << endl;
    cout << "Name: " << shape->getName() << endl;
    cout << "Area: " << shape->area() << endl;
    cout << "Perimeter: " << shape->perimeter() << endl;
}

int main() {
    cout << "===== Inheritance and Polymorphism Demo =====" << endl;
    
    // Create objects of derived classes
    Circle circle(5.0, "Red");
    Rectangle rectangle(4.0, 6.0, "Blue");
    Triangle triangle(3.0, 4.0, 5.0, "Green");
    
    cout << "\n===== Shape Details =====" << endl;
    
    // Display information using virtual functions
    cout << "\n----- Circle -----" << endl;
    circle.display();
    
    cout << "\n----- Rectangle -----" << endl;
    rectangle.display();
    
    cout << "\n----- Triangle -----" << endl;
    triangle.display();
    
    cout << "\n===== Polymorphism with Base Class Pointers =====" << endl;
    
    // Array of base class pointers
    Shape* shapes[3];
    shapes[0] = &circle;
    shapes[1] = &rectangle;
    shapes[2] = &triangle;
    
    // Process all shapes through base class pointers
    for (int i = 0; i < 3; i++) {
        displayShapeInfo(shapes[i]);
    }
    
    cout << "\n===== Dynamic Object Creation =====" << endl;
    
    // Dynamic memory allocation for derived class objects
    Shape* dynamicCircle = new Circle(7.5, "Yellow");
    Shape* dynamicRectangle = new Rectangle(8.0, 3.0, "Purple");
    
    // Use virtual functions through base class pointers
    dynamicCircle->display();
    dynamicRectangle->display();
    
    // Clean up dynamic memory
    delete dynamicCircle;
    delete dynamicRectangle;
    
    cout << "\nEnd of program" << endl;
    return 0;
}