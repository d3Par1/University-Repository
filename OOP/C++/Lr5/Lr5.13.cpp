#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// First namespace: Math
namespace Math {
    // Constants
    const double PI = 3.14159265358979323846;
    const double E = 2.71828182845904523536;
    
    // Simple functions
    double square(double x) {
        return x * x;
    }
    
    double cube(double x) {
        return x * x * x;
    }
    
    double power(double base, int exponent) {
        double result = 1.0;
        for (int i = 0; i < exponent; ++i) {
            result *= base;
        }
        return result;
    }
    
    // Nested namespace: Geometry
    namespace Geometry {
        // Circle functions
        double circleArea(double radius) {
            return PI * radius * radius;
        }
        
        double circleCircumference(double radius) {
            return 2 * PI * radius;
        }
        
        // Rectangle functions
        double rectangleArea(double width, double height) {
            return width * height;
        }
        
        double rectanglePerimeter(double width, double height) {
            return 2 * (width + height);
        }
        
        // Shape structure for ADL demonstration
        struct Shape {
            string name;
            double area;
            
            Shape(const string& n, double a) : name(n), area(a) {}
        };
        
        // Function for ADL demonstration
        void describe(const Shape& shape) {
            cout << "Geometry::describe - Shape: " << shape.name 
                 << ", Area: " << shape.area << endl;
        }
    }
    
    // Nested namespace: Statistics
    namespace Statistics {
        double mean(const vector<double>& values) {
            if (values.empty()) return 0.0;
            
            double sum = 0.0;
            for (double val : values) {
                sum += val;
            }
            return sum / values.size();
        }
        
        double median(vector<double> values) {
            if (values.empty()) return 0.0;
            
            // Sort the vector
            sort(values.begin(), values.end());
            
            // Return the median
            if (values.size() % 2 == 0) {
                // Even number of elements
                size_t mid = values.size() / 2;
                return (values[mid - 1] + values[mid]) / 2.0;
            }
            else {
                // Odd number of elements
                return values[values.size() / 2];
            }
        }
        
        // Dataset structure for ADL demonstration
        struct Dataset {
            string name;
            vector<double> data;
            
            Dataset(const string& n, const vector<double>& d) : name(n), data(d) {}
        };
        
        // Function for ADL demonstration
        void analyze(const Dataset& dataset) {
            cout << "Statistics::analyze - Dataset: " << dataset.name 
                 << ", Mean: " << mean(dataset.data)
                 << ", Median: " << median(dataset.data) << endl;
        }
    }
}

// Second namespace: Graphics
namespace Graphics {
    // Color structure
    struct Color {
        int red;
        int green;
        int blue;
        
        Color(int r = 0, int g = 0, int b = 0) : red(r), green(g), blue(b) {}
        
        string toString() const {
            return "RGB(" + to_string(red) + ", " 
                          + to_string(green) + ", " 
                          + to_string(blue) + ")";
        }
    };
    
    // Point structure
    struct Point {
        double x;
        double y;
        
        Point(double xVal = 0.0, double yVal = 0.0) : x(xVal), y(yVal) {}
        
        string toString() const {
            return "(" + to_string(x) + ", " + to_string(y) + ")";
        }
    };
    
    // Function for ADL demonstration
    void print(const Color& color) {
        cout << "Graphics::print - Color: " << color.toString() << endl;
    }
    
    void print(const Point& point) {
        cout << "Graphics::print - Point: " << point.toString() << endl;
    }
    
    // Shape class hierarchy
    class Shape {
    protected:
        Point position;
        Color color;
        
    public:
        Shape(const Point& pos = Point(), const Color& col = Color())
            : position(pos), color(col) {}
        
        virtual ~Shape() {}
        
        virtual void draw() const {
            cout << "Drawing a shape at " << position.toString() 
                 << " with color " << color.toString() << endl;
        }
        
        virtual double area() const = 0;
        
        // Getters and setters
        Point getPosition() const { return position; }
        void setPosition(const Point& pos) { position = pos; }
        
        Color getColor() const { return color; }
        void setColor(const Color& col) { color = col; }
    };
    
    // Circle class
    class Circle : public Shape {
    private:
        double radius;
        
    public:
        Circle(const Point& center = Point(), double r = 1.0, const Color& col = Color())
            : Shape(center, col), radius(r) {}
        
        void draw() const override {
            cout << "Drawing a circle at " << position.toString() 
                 << " with radius " << radius
                 << " and color " << color.toString() << endl;
        }
        
        double area() const override {
            return Math::PI * radius * radius;
        }
        
        // Getters and setters
        double getRadius() const { return radius; }
        void setRadius(double r) { radius = r; }
    };
    
    // Rectangle class
    class Rectangle : public Shape {
    private:
        double width;
        double height;
        
    public:
        Rectangle(const Point& topLeft = Point(), double w = 1.0, double h = 1.0, 
                  const Color& col = Color())
            : Shape(topLeft, col), width(w), height(h) {}
        
        void draw() const override {
            cout << "Drawing a rectangle at " << position.toString() 
                 << " with width " << width << " and height " << height
                 << " and color " << color.toString() << endl;
        }
        
        double area() const override {
            return width * height;
        }
        
        // Getters and setters
        double getWidth() const { return width; }
        void setWidth(double w) { width = w; }
        
        double getHeight() const { return height; }
        void setHeight(double h) { height = h; }
    };
}

// Third namespace: Utils
namespace Utils {
    // Function to generate a vector of random values
    vector<double> generateRandomValues(int count, double min, double max) {
        vector<double> values;
        values.reserve(count);
        
        // Seed the random number generator
        srand(static_cast<unsigned int>(time(nullptr)));
        
        for (int i = 0; i < count; ++i) {
            double val = min + static_cast<double>(rand()) / RAND_MAX * (max - min);
            values.push_back(val);
        }
        
        return values;
    }
    
    // Timer class
    class Timer {
    private:
        clock_t startTime;
        string label;
        
    public:
        Timer(const string& l = "Timer") : label(l) {
            startTime = clock();
            cout << label << " started." << endl;
        }
        
        ~Timer() {
            double elapsed = static_cast<double>(clock() - startTime) / CLOCKS_PER_SEC;
            cout << label << " stopped. Elapsed time: " << elapsed << " seconds." << endl;
        }
    };
    
    // Function to print a vector
    template <typename T>
    void printVector(const vector<T>& vec, const string& label = "Vector") {
        cout << label << ": [";
        for (size_t i = 0; i < vec.size(); ++i) {
            cout << vec[i];
            if (i < vec.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
}

// Inline namespace example (C++11 feature)
namespace Version {
    inline namespace V2 {
        void feature() {
            cout << "Using the latest version (V2) of feature()" << endl;
        }
        
        class API {
        public:
            void method() {
                cout << "Using the latest version (V2) of API::method()" << endl;
            }
        };
    }
    
    namespace V1 {
        void feature() {
            cout << "Using the legacy version (V1) of feature()" << endl;
        }
        
        class API {
        public:
            void method() {
                cout << "Using the legacy version (V1) of API::method()" << endl;
            }
        };
    }
}

// Anonymous namespace example
namespace {
    // Variables in anonymous namespace have internal linkage
    int privateCounter = 0;
    
    void incrementCounter() {
        ++privateCounter;
        cout << "Private counter incremented to: " << privateCounter << endl;
    }
}

// Argument-Dependent Lookup (ADL) demonstration
namespace ADL {
    struct Data {
        int value;
        
        Data(int v = 0) : value(v) {}
    };
    
    // Function to be found by ADL
    void process(const Data& data) {
        cout << "ADL::process - Processing data with value: " << data.value << endl;
    }
}

// Using directives and declarations
using Math::square;
using Math::cube;
// using namespace Math::Geometry;  // Commenting out to show qualified access

int main() {
    cout << "===== Namespaces and ADL Demo =====" << endl;
    
    // Part 1: Basic namespace usage
    cout << "\n1. Basic Namespace Usage:" << endl;
    {
        // Accessing namespace members with qualification
        cout << "Math::PI = " << Math::PI << endl;
        cout << "Math::square(4) = " << Math::square(4) << endl;
        
        // Using declaration
        cout << "square(5) = " << square(5) << " (using declaration)" << endl;
        cout << "cube(3) = " << cube(3) << " (using declaration)" << endl;
        
        // Accessing nested namespaces
        cout << "Math::Geometry::circleArea(2) = " 
             << Math::Geometry::circleArea(2) << endl;
        
        // Using namespace alias
        namespace MG = Math::Geometry;
        cout << "MG::rectangleArea(3, 4) = " << MG::rectangleArea(3, 4) 
             << " (using namespace alias)" << endl;
    }
    
    // Part 2: Working with multiple namespaces
    cout << "\n2. Working with Multiple Namespaces:" << endl;
    {
        // Create graphics objects
        Graphics::Point p1(2.0, 3.0);
        Graphics::Color red(255, 0, 0);
        
        // Create shapes
        Graphics::Circle circle(p1, 5.0, red);
        Graphics::Rectangle rect(Graphics::Point(1.0, 1.0), 4.0, 3.0, 
                                 Graphics::Color(0, 0, 255));
        
        // Draw shapes
        circle.draw();
        rect.draw();
        
        // Calculate areas using functions from different namespaces
        cout << "Circle area using Graphics::Circle::area(): " 
             << circle.area() << endl;
        cout << "Circle area using Math::Geometry::circleArea(): " 
             << Math::Geometry::circleArea(circle.getRadius()) << endl;
        
        cout << "Rectangle area using Graphics::Rectangle::area(): " 
             << rect.area() << endl;
        cout << "Rectangle area using Math::Geometry::rectangleArea(): " 
             << Math::Geometry::rectangleArea(rect.getWidth(), rect.getHeight()) << endl;
    }
    
    // Part 3: Utility functions and classes
    cout << "\n3. Utility Functions and Classes:" << endl;
    {
        // Using the timer class
        Utils::Timer timer("Main section timer");
        
        // Generate random values
        vector<double> values = Utils::generateRandomValues(10, 0.0, 100.0);
        
        // Print the vector
        Utils::printVector(values, "Random values");
        
        // Calculate statistics
        double mean = Math::Statistics::mean(values);
        double median = Math::Statistics::median(values);
        
        cout << "Mean: " << mean << endl;
        cout << "Median: " << median << endl;
    }
    
    // Part 4: Argument-Dependent Lookup (ADL)
    cout << "\n4. Argument-Dependent Lookup (ADL):" << endl;
    {
        // Create objects from different namespaces
        ADL::Data data(42);
        Math::Geometry::Shape shape("Circle", 78.5);
        Math::Statistics::Dataset dataset("Temperature", {23.5, 25.1, 24.8, 26.2, 22.9});
        Graphics::Color color(128, 128, 128);
        Graphics::Point point(5.0, 7.0);
        
        // Call functions without namespace qualification (ADL)
        process(data);           // Finds ADL::process
        describe(shape);         // Finds Math::Geometry::describe
        analyze(dataset);        // Finds Math::Statistics::analyze
        print(color);            // Finds Graphics::print
        print(point);            // Finds Graphics::print
    }
    
    // Part 5: Inline namespaces
    cout << "\n5. Inline Namespaces:" << endl;
    {
        // Access through version namespace (gets inline version)
        Version::feature();
        Version::API api1;
        api1.method();
        
        // Explicitly access older version
        Version::V1::feature();
        Version::V1::API api2;
        api2.method();
    }
    
    // Part 6: Anonymous namespaces
    cout << "\n6. Anonymous Namespaces:" << endl;
    {
        // Access members of an anonymous namespace
        incrementCounter();
        incrementCounter();
        incrementCounter();
    }
    
    cout << "\nEnd of program" << endl;
    return 0;
}