#include <iostream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <cxxabi.h> // For demangling type names
#include <memory>
#include <tuple>
using namespace std;

// Helper function to get human-readable type name
template <typename T>
string type_name() {
    int status;
    char* demangled = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    
    string result;
    if (status == 0) {
        result = demangled;
    } else {
        result = typeid(T).name();
    }
    
    free(demangled);
    return result;
}

// Part 1: Variadic templates for print function
// Base case: no arguments
void print() {
    cout << endl;
}

// Variadic template for print
template <typename T, typename... Args>
void print(const T& first, const Args&... rest) {
    cout << first;
    if (sizeof...(rest) > 0) {
        cout << ", ";
    }
    print(rest...);
}

// Part 2: Tuple printer with index sequence
// Helper to print tuple
template <typename Tuple, size_t... Is>
void print_tuple_impl(const Tuple& t, index_sequence<Is...>) {
    print(get<Is>(t)...);
}

// Print tuple
template <typename... Args>
void print_tuple(const tuple<Args...>& t) {
    cout << "Tuple(";
    print_tuple_impl(t, index_sequence_for<Args...>{});
    cout << ")" << endl;
}

// Part 3: Compile-time factorial calculation
template <unsigned int N>
struct Factorial {
    static constexpr unsigned int value = N * Factorial<N - 1>::value;
};

// Base case specialization
template <>
struct Factorial<0> {
    static constexpr unsigned int value = 1;
};

// Part 4: Compile-time Fibonacci calculation
template <unsigned int N>
struct Fibonacci {
    static constexpr unsigned int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

// Base case specializations
template <>
struct Fibonacci<0> {
    static constexpr unsigned int value = 0;
};

template <>
struct Fibonacci<1> {
    static constexpr unsigned int value = 1;
};

// Part 5: Type traits and SFINAE

// Check if a type has a toString method
template <typename T>
class has_toString {
private:
    // Two overloads for yes/no type
    typedef char yes_type;
    typedef long no_type;
    
    // If T has toString, this will be chosen due to exact match
    template <typename U>
    static yes_type test(decltype(&U::toString));
    
    // Otherwise, this will be chosen
    template <typename U>
    static no_type test(...);
    
public:
    // Value is true if the sizeof test<T>(0) is sizeof(yes_type)
    static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(yes_type);
};

// Example classes for testing
class WithToString {
public:
    string toString() const {
        return "WithToString object";
    }
};

class WithoutToString {
public:
    void someOtherMethod() {}
};

// Function that uses SFINAE to call toString if available
template <typename T>
typename enable_if<has_toString<T>::value, string>::type
getStringRepresentation(const T& obj) {
    return obj.toString();
}

// Overload for types without toString
template <typename T>
typename enable_if<!has_toString<T>::value, string>::type
getStringRepresentation(const T& obj) {
    return "Object does not have toString method";
}

// Part 6: Variadic template for sum
// Base case
template <typename T>
T sum(T value) {
    return value;
}

// Recursive case
template <typename T, typename... Args>
T sum(T first, Args... args) {
    return first + sum<T>(args...);
}

// Part 7: Recursive templates for power calculation
// Calculate base^exponent at compile time
template <typename T, unsigned int exponent>
struct Power {
    static T calculate(T base) {
        return base * Power<T, exponent - 1>::calculate(base);
    }
};

// Base case
template <typename T>
struct Power<T, 0> {
    static T calculate(T base) {
        return static_cast<T>(1);
    }
};

// Part 8: Type list and operations
// Define a type list
template <typename... Ts>
struct TypeList {};

// Calculate the length of a TypeList
template <typename... Ts>
struct Length {
    static constexpr size_t value = sizeof...(Ts);
};


// Get the first type in a TypeList
template <typename T, typename... Ts>
struct First {
    using type = T;
};

template <typename... Ts>
struct First<TypeList<Ts...>> {
    using type = typename First<Ts...>::type;
};

template <typename T>
struct First<TypeList<T>> {
    using type = T;
};

// Get the last type in a TypeList
template <typename T>
struct Last {
    using type = T;
};

template <typename T, typename... Ts>
struct Last<TypeList<T, Ts...>> {
    using type = typename Last<TypeList<Ts...>>::type;
};

template <typename T>
struct Last<TypeList<T>> {
    using type = T;
};

// Part 9: Fold expressions (C++17 feature)
// Sum using fold expression
template <typename... Args>
auto foldSum(Args... args) {
    return (... + args); // Unary left fold
}

// Checking if all values are true
template <typename... Args>
bool allTrue(Args... args) {
    return (... && args); // Unary left fold
}

// Checking if any value is true
template <typename... Args>
bool anyTrue(Args... args) {
    return (... || args); // Unary left fold
}

// Part 10: Perfect forwarding factory
// Factory function that perfectly forwards constructor arguments
template <typename T, typename... Args>
unique_ptr<T> my_make_unique(Args&&... args) {
    return unique_ptr<T>(new T(forward<Args>(args)...));
}

// Example class for the factory
class Person {
private:
    string name;
    int age;
    
public:
    Person(string n, int a) : name(move(n)), age(a) {
        cout << "Person constructed: " << name << ", " << age << endl;
    }
    
    string getName() const { return name; }
    int getAge() const { return age; }
};

int main() {
    cout << "===== Variadic Templates and Template Metaprogramming Demo =====" << endl;
    
    // Part 1: Variadic templates for print function
    cout << "\n1. Variadic Print Function:" << endl;
    print("Hello", 42, 3.14, 'A', true);
    
    // Part 2: Tuple printer
    cout << "\n2. Tuple Printer:" << endl;
    auto t1 = make_tuple(1, "test", 3.14, 'X');
    print_tuple(t1);
    
    // Part 3: Compile-time factorial
    cout << "\n3. Compile-Time Factorial:" << endl;
    cout << "Factorial<5>::value = " << Factorial<5>::value << endl;
    cout << "Factorial<10>::value = " << Factorial<10>::value << endl;
    
    // Part 4: Compile-time Fibonacci
    cout << "\n4. Compile-Time Fibonacci:" << endl;
    cout << "Fibonacci numbers:" << endl;
    cout << "Fibonacci<0>::value = " << Fibonacci<0>::value << endl;
    cout << "Fibonacci<1>::value = " << Fibonacci<1>::value << endl;
    cout << "Fibonacci<2>::value = " << Fibonacci<2>::value << endl;
    cout << "Fibonacci<3>::value = " << Fibonacci<3>::value << endl;
    cout << "Fibonacci<4>::value = " << Fibonacci<4>::value << endl;
    cout << "Fibonacci<5>::value = " << Fibonacci<5>::value << endl;
    cout << "Fibonacci<10>::value = " << Fibonacci<10>::value << endl;
    cout << "Fibonacci<20>::value = " << Fibonacci<20>::value << endl;
    
    // Part 5: Type traits and SFINAE
    cout << "\n5. Type Traits and SFINAE:" << endl;
    WithToString withToString;
    WithoutToString withoutToString;
    
    cout << "has_toString<WithToString>::value = " << has_toString<WithToString>::value << endl;
    cout << "has_toString<WithoutToString>::value = " << has_toString<WithoutToString>::value << endl;
    cout << "has_toString<int>::value = " << has_toString<int>::value << endl;
    
    cout << "getStringRepresentation(withToString) = " << getStringRepresentation(withToString) << endl;
    cout << "getStringRepresentation(withoutToString) = " << getStringRepresentation(withoutToString) << endl;
    cout << "getStringRepresentation(42) = " << getStringRepresentation(42) << endl;
    
    // Part 6: Variadic template for sum
    cout << "\n6. Variadic Sum:" << endl;
    cout << "sum(1, 2, 3, 4, 5) = " << sum(1, 2, 3, 4, 5) << endl;
    cout << "sum(1.1, 2.2, 3.3) = " << sum(1.1, 2.2, 3.3) << endl;
    
    // Part 7: Recursive templates for power
    cout << "\n7. Recursive Templates for Power:" << endl;
    cout << "Power<int, 0>::calculate(5) = " << Power<int, 0>::calculate(5) << endl;
    cout << "Power<int, 1>::calculate(5) = " << Power<int, 1>::calculate(5) << endl;
    cout << "Power<int, 2>::calculate(5) = " << Power<int, 2>::calculate(5) << endl;
    cout << "Power<int, 5>::calculate(2) = " << Power<int, 5>::calculate(2) << endl;
    cout << "Power<double, 3>::calculate(2.5) = " << Power<double, 3>::calculate(2.5) << endl;
    
    // Part 8: Type list and operations
    cout << "\n8. Type List and Operations:" << endl;
    using MyTypeList = TypeList<int, double, char, string>;
    
    cout << "Length<MyTypeList>::value = " << Length<MyTypeList>::value << endl;
    
    using FirstType = typename First<MyTypeList>::type;
    cout << "First<MyTypeList>::type = " << type_name<FirstType>() << endl;
    
    using LastType = typename Last<MyTypeList>::type;
    cout << "Last<MyTypeList>::type = " << type_name<LastType>() << endl;
    
    // Part 9: Fold expressions
    cout << "\n9. Fold Expressions:" << endl;
    cout << "foldSum(1, 2, 3, 4, 5) = " << foldSum(1, 2, 3, 4, 5) << endl;
    cout << "allTrue(true, true, true) = " << allTrue(true, true, true) << endl;
    cout << "allTrue(true, false, true) = " << allTrue(true, false, true) << endl;
    cout << "anyTrue(false, false, true) = " << anyTrue(false, false, true) << endl;
    cout << "anyTrue(false, false, false) = " << anyTrue(false, false, false) << endl;
    
    // Part 10: Perfect forwarding factory
    cout << "\n10. Perfect Forwarding Factory:" << endl;
    auto person = my_make_unique<Person>("Alice", 30);
    cout << "Person name: " << person->getName() << endl;
    cout << "Person age: " << person->getAge() << endl;
    
    cout << "\nEnd of program" << endl;
    return 0;
}