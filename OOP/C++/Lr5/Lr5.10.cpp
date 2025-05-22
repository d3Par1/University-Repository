#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <functional>
#include <cmath>
using namespace std;

// Simple function object (functor) for addition
class Adder {
private:
    int value;
    
public:
    // Constructor
    Adder(int val = 0) : value(val) {
        cout << "Adder constructed with value: " << value << endl;
    }
    
    // Function call operator
    int operator()(int x) const {
        return x + value;
    }
    
    // Overload for summing two values
    int operator()(int x, int y) const {
        return x + y + value;
    }
};

// Function object for multiplication
class Multiplier {
private:
    int factor;
    
public:
    // Constructor
    Multiplier(int f = 1) : factor(f) {}
    
    // Function call operator
    int operator()(int x) const {
        return x * factor;
    }
};

// Functor that maintains internal state
class Counter {
private:
    int count;
    
public:
    // Constructor
    Counter() : count(0) {}
    
    // Function call operator
    int operator()() {
        return ++count;
    }
    
    // Reset counter
    void reset() {
        count = 0;
    }
    
    // Get current count
    int getCount() const {
        return count;
    }
};

// Functor for filtering even numbers
class IsEven {
public:
    bool operator()(int x) const {
        return x % 2 == 0;
    }
};

// Functor for filtering odd numbers
class IsOdd {
public:
    bool operator()(int x) const {
        return x % 2 != 0;
    }
};

// Template functor for comparing values
template <typename T>
class IsGreaterThan {
private:
    T threshold;
    
public:
    // Constructor
    IsGreaterThan(const T& t) : threshold(t) {}
    
    // Function call operator
    bool operator()(const T& value) const {
        return value > threshold;
    }
};

// Complex functor example: string matcher
class StringMatcher {
private:
    string pattern;
    bool caseSensitive;
    
public:
    // Constructor
    StringMatcher(const string& p, bool cs = true) 
        : pattern(p), caseSensitive(cs) {}
    
    // Function call operator
    bool operator()(const string& str) const {
        if (caseSensitive) {
            return str.find(pattern) != string::npos;
        }
        else {
            // Case-insensitive matching
            string lowerStr = str;
            string lowerPattern = pattern;
            
            // Convert to lowercase
            transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
            transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
            
            return lowerStr.find(lowerPattern) != string::npos;
        }
    }
};

// Function to display a vector
template <typename T>
void displayVector(const string& label, const vector<T>& vec) {
    cout << label << ": ";
    for (const auto& elem : vec) {
        cout << elem << " ";
    }
    cout << endl;
}

// Traditional function for comparison with functors
int add(int x, int y) {
    return x + y;
}

int multiply(int x, int y) {
    return x * y;
}

bool isEven(int x) {
    return x % 2 == 0;
}

// Function to process a vector with a unary function object
template <typename T, typename Func>
vector<T> processVector(const vector<T>& input, Func func) {
    vector<T> result;
    result.reserve(input.size());
    
    for (const auto& elem : input) {
        result.push_back(func(elem));
    }
    
    return result;
}

// Function to filter a vector using a predicate
template <typename T, typename Pred>
vector<T> filterVector(const vector<T>& input, Pred pred) {
    vector<T> result;
    
    for (const auto& elem : input) {
        if (pred(elem)) {
            result.push_back(elem);
        }
    }
    
    return result;
}

int main() {
    cout << "===== Function Objects and Predicates Demo =====" << endl;
    
    // Part 1: Basic function objects
    cout << "\n1. Basic Function Objects:" << endl;
    {
        // Create an Adder object
        Adder add5(5);
        
        cout << "add5(10) = " << add5(10) << endl;
        cout << "add5(10, 20) = " << add5(10, 20) << endl;
        
        // Create a Multiplier object
        Multiplier mult3(3);
        
        cout << "mult3(10) = " << mult3(10) << endl;
        
        // Use Counter object
        Counter counter;
        cout << "counter() = " << counter() << endl;
        cout << "counter() = " << counter() << endl;
        cout << "counter() = " << counter() << endl;
        cout << "Current count: " << counter.getCount() << endl;
        
        counter.reset();
        cout << "After reset, counter() = " << counter() << endl;
    }
    
    // Part 2: Using functors with STL algorithms
    cout << "\n2. Functors with STL Algorithms:" << endl;
    {
        vector<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        displayVector("Original numbers", numbers);
        
        // Transform using Adder
        vector<int> added;
        transform(numbers.begin(), numbers.end(), back_inserter(added), Adder(100));
        displayVector("After adding 100", added);
        
        // Transform using Multiplier
        vector<int> multiplied;
        transform(numbers.begin(), numbers.end(), back_inserter(multiplied), Multiplier(10));
        displayVector("After multiplying by 10", multiplied);
        
        // Count elements using predicates
        int evenCount = count_if(numbers.begin(), numbers.end(), IsEven());
        int oddCount = count_if(numbers.begin(), numbers.end(), IsOdd());
        
        cout << "Number of even elements: " << evenCount << endl;
        cout << "Number of odd elements: " << oddCount << endl;
        
        // Filter using remove_if and erase
        vector<int> evenOnly = numbers;
        evenOnly.erase(
            remove_if(evenOnly.begin(), evenOnly.end(), IsOdd()),
            evenOnly.end()
        );
        displayVector("Even numbers only", evenOnly);
        
        // Filter using template predicate
        vector<int> greaterThan5;
        copy_if(numbers.begin(), numbers.end(), 
                back_inserter(greaterThan5),
                IsGreaterThan<int>(5));
        displayVector("Numbers greater than 5", greaterThan5);
    }
    
    // Part 3: Custom filtering with function objects
    cout << "\n3. Custom Filtering with Function Objects:" << endl;
    {
        vector<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        // Filter using our custom templated function
        vector<int> evenNumbers = filterVector(numbers, IsEven());
        displayVector("Even numbers", evenNumbers);
        
        vector<int> oddNumbers = filterVector(numbers, IsOdd());
        displayVector("Odd numbers", oddNumbers);
        
        // Process vector with our custom function
        vector<int> processed = processVector(numbers, Adder(10));
        displayVector("Numbers + 10", processed);
        
        // Combining multiple operations
        vector<int> complex = processVector(
            filterVector(numbers, IsGreaterThan<int>(5)),
            Multiplier(2)
        );
        displayVector("(Numbers > 5) * 2", complex);
    }
    
    // Part 4: String matching functor example
    cout << "\n4. String Matching Functor Example:" << endl;
    {
        vector<string> words{
            "apple", "banana", "Orange", "pineapple", 
            "grape", "Applesauce", "pear", "kiwi"
        };
        displayVector("Original words", words);
        
        // Case-sensitive matching
        StringMatcher appleMatcherCS("apple", true);
        vector<string> applesCS = filterVector(words, appleMatcherCS);
        displayVector("Words containing 'apple' (case-sensitive)", applesCS);
        
        // Case-insensitive matching
        StringMatcher appleMatcherCI("apple", false);
        vector<string> applesCI = filterVector(words, appleMatcherCI);
        displayVector("Words containing 'apple' (case-insensitive)", applesCI);
    }
    
    // Part 5: Comparison with traditional functions
    cout << "\n5. Comparison with Traditional Functions:" << endl;
    {
        // Using function pointers
        cout << "add(5, 3) = " << add(5, 3) << endl;
        cout << "multiply(5, 3) = " << multiply(5, 3) << endl;
        
        // Can also use with STL algorithms
        vector<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        vector<int> evenNumbers;
        copy_if(numbers.begin(), numbers.end(), 
                back_inserter(evenNumbers),
                isEven);  // Using a function pointer
        displayVector("Even numbers (using function pointer)", evenNumbers);
    }
    
    // Part 6: Using lambda expressions (modern C++ approach)
    cout << "\n6. Using Lambda Expressions:" << endl;
    {
        vector<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        // Simple lambda
        auto isEvenLambda = [](int x) { return x % 2 == 0; };
        vector<int> evenNumbers = filterVector(numbers, isEvenLambda);
        displayVector("Even numbers (using lambda)", evenNumbers);
        
        // Lambda with capture
        int threshold = 7;
        auto greaterThanThreshold = [threshold](int x) { return x > threshold; };
        vector<int> filtered = filterVector(numbers, greaterThanThreshold);
        displayVector("Numbers > " + to_string(threshold), filtered);
        
        // Mutable lambda maintaining state
        auto counterLambda = [count = 0]() mutable { return ++count; };
        cout << "counterLambda() = " << counterLambda() << endl;
        cout << "counterLambda() = " << counterLambda() << endl;
        cout << "counterLambda() = " << counterLambda() << endl;
        
        // Direct use with algorithm
        vector<int> multiplied;
        transform(numbers.begin(), numbers.end(), 
                 back_inserter(multiplied),
                 [factor = 5](int x) { return x * factor; });
        displayVector("Numbers * 5 (lambda)", multiplied);
    }
    
    // Part 7: Using std::function
    cout << "\n7. Using std::function:" << endl;
    {
        // Define function objects of type std::function
        function<int(int)> doubler = [](int x) { return x * 2; };
        function<bool(int)> isDivisibleBy3 = [](int x) { return x % 3 == 0; };
        
        cout << "doubler(5) = " << doubler(5) << endl;
        cout << "isDivisibleBy3(9) = " << isDivisibleBy3(9) << endl;
        cout << "isDivisibleBy3(10) = " << isDivisibleBy3(10) << endl;
        
        // Store different callables in std::function
        function<int(int, int)> operation;
        
        operation = add;  // Function pointer
        cout << "operation(5, 3) [add] = " << operation(5, 3) << endl;
        
        operation = [](int x, int y) { return x * y; };  // Lambda
        cout << "operation(5, 3) [multiply] = " << operation(5, 3) << endl;
        
        // Creating an array of functions
        vector<function<double(double)>> mathOps = {
            [](double x) { return x * x; },            // Square
            [](double x) { return sqrt(x); },          // Square root
            [](double x) { return sin(x); },           // Sine
            [](double x) { return cos(x); }            // Cosine
        };
        
        double value = 4.0;
        cout << "Operations on " << value << ":" << endl;
        cout << "  Square: " << mathOps[0](value) << endl;
        cout << "  Square root: " << mathOps[1](value) << endl;
        cout << "  Sine: " << mathOps[2](value) << endl;
        cout << "  Cosine: " << mathOps[3](value) << endl;
    }
    
    cout << "\nEnd of program" << endl;
    return 0;
}