#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <chrono>
using namespace std;

// Timer class for performance comparison
class Timer {
private:
    chrono::high_resolution_clock::time_point start_time;
    string label;
    
public:
    // Constructor
    Timer(const string& l) : label(l) {
        start_time = chrono::high_resolution_clock::now();
    }
    
    // Destructor
    ~Timer() {
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
        cout << label << ": " << duration << " microseconds" << endl;
    }
};

// Resource class to demonstrate move semantics
class Resource {
private:
    string name;
    int* data;
    size_t size;
    
    // Helper method to allocate memory
    void allocate(size_t s) {
        size = s;
        data = new int[size];
        for (size_t i = 0; i < size; i++) {
            data[i] = static_cast<int>(i);
        }
    }
    
public:
    // Default constructor
    Resource() : name(""), data(nullptr), size(0) {
        cout << "Default constructor called" << endl;
    }
    
    // Parameterized constructor
    Resource(const string& n, size_t s) : name(n), data(nullptr), size(0) {
        cout << "Parameterized constructor for " << name << endl;
        allocate(s);
    }
    
    // Copy constructor
    Resource(const Resource& other) : name(other.name + " (copy)"), data(nullptr), size(0) {
        cout << "Copy constructor for " << name << endl;
        allocate(other.size);
        // Deep copy the data
        for (size_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
    
    // Move constructor
    Resource(Resource&& other) noexcept : name(move(other.name)), data(other.data), size(other.size) {
        cout << "Move constructor for " << name << endl;
        // Take ownership of the resources
        other.data = nullptr;
        other.size = 0;
    }
    
    // Copy assignment operator
    Resource& operator=(const Resource& other) {
        cout << "Copy assignment operator for " << name << endl;
        if (this != &other) {
            // Clean up existing resources
            delete[] data;
            data = nullptr;
            
            // Copy name (with indicator)
            name = other.name + " (copy assigned)";
            
            // Deep copy the data
            allocate(other.size);
            for (size_t i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    // Move assignment operator
    Resource& operator=(Resource&& other) noexcept {
        cout << "Move assignment operator for " << name << endl;
        if (this != &other) {
            // Clean up existing resources
            delete[] data;
            
            // Move the resources
            name = move(other.name);
            data = other.data;
            size = other.size;
            
            // Leave the source in a valid state
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    // Destructor
    ~Resource() {
        cout << "Destructor for " << (name.empty() ? "unnamed resource" : name) << endl;
        delete[] data;
    }
    
    // Get name
    const string& getName() const {
        return name;
    }
    
    // Get size
    size_t getSize() const {
        return size;
    }
    
    // Sum the data (to show we can use the data)
    int sum() const {
        int total = 0;
        for (size_t i = 0; i < size; i++) {
            total += data[i];
        }
        return total;
    }
};

// Function that takes an lvalue reference
void processByLvalueRef(const Resource& res) {
    cout << "Processing by lvalue reference: " << res.getName() 
         << ", Size: " << res.getSize() 
         << ", Sum: " << res.sum() << endl;
}

// Function that takes an rvalue reference
void processByRvalueRef(Resource&& res) {
    cout << "Processing by rvalue reference: " << res.getName() 
         << ", Size: " << res.getSize() 
         << ", Sum: " << res.sum() << endl;
    
    // We can modify res here since we own it
    Resource newOwner = move(res);
    cout << "Moved resource to newOwner: " << newOwner.getName() << endl;
}

// Template function with perfect forwarding
template <typename T>
void perfectForward(T&& arg) {
    cout << "Perfect forwarding: ";
    
    // Forward the argument with its original value category preserved
    processByLvalueRef(forward<T>(arg));
}

// Factory function to create a Resource using perfect forwarding
template <typename... Args>
Resource createResource(Args&&... args) {
    return Resource(forward<Args>(args)...);
}

// Function to demonstrate copy elision and RVO (Return Value Optimization)
Resource createAndReturnResource(const string& name, size_t size) {
    cout << "Creating resource in function..." << endl;
    Resource res(name, size);
    return res; // Compiler can optimize this (RVO)
}

// Class with move-only semantics
class MoveOnly {
private:
    int* data;
    
public:
    // Constructor
    MoveOnly() : data(new int(0)) {
        cout << "MoveOnly default constructor" << endl;
    }
    
    // Move constructor
    MoveOnly(MoveOnly&& other) noexcept : data(other.data) {
        cout << "MoveOnly move constructor" << endl;
        other.data = nullptr;
    }
    
    // Move assignment
    MoveOnly& operator=(MoveOnly&& other) noexcept {
        cout << "MoveOnly move assignment" << endl;
        if (this != &other) {
            delete data;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }
    
    // Delete copy constructor and assignment
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    
    // Destructor
    ~MoveOnly() {
        cout << "MoveOnly destructor" << endl;
        delete data;
    }
    
    // Get data
    int getValue() const {
        return data ? *data : 0;
    }
    
    // Set data
    void setValue(int val) {
        if (data) {
            *data = val;
        }
    }
};

// Simple string wrapper to demonstrate string move operations
class StringWrapper {
private:
    string str;
    
public:
    // Constructor
    StringWrapper(const string& s) : str(s) {
        cout << "StringWrapper constructor with copy" << endl;
    }
    
    // Constructor with move semantics
    StringWrapper(string&& s) : str(move(s)) {
        cout << "StringWrapper constructor with move" << endl;
    }
    
    // Get string
    const string& getString() const {
        return str;
    }
};

int main() {
    cout << "===== Move Semantics and Perfect Forwarding Demo =====" << endl;
    
    // Part 1: Basic move semantics
    cout << "\n1. Basic Move Semantics:" << endl;
    {
        cout << "\nCreating the original resource:" << endl;
        Resource original("Original", 1000);
        
        cout << "\nCopying the resource:" << endl;
        Resource copy(original);
        
        cout << "\nMoving the resource:" << endl;
        Resource moved(move(original));
        
        cout << "\nState after operations:" << endl;
        cout << "Original name: " << original.getName() << ", Size: " << original.getSize() << endl;
        cout << "Copy name: " << copy.getName() << ", Size: " << copy.getSize() << endl;
        cout << "Moved name: " << moved.getName() << ", Size: " << moved.getSize() << endl;
    }
    
    // Part 2: Move vs. Copy performance
    cout << "\n2. Move vs. Copy Performance:" << endl;
    {
        const int SIZE = 10000000;
        
        cout << "\nCreating a large resource:" << endl;
        Resource largeResource("Large", SIZE);
        
        cout << "\nMeasuring copy performance:" << endl;
        {
            Timer copyTimer("Copy time");
            Resource copiedResource(largeResource);
        }
        
        cout << "\nMeasuring move performance:" << endl;
        {
            Timer moveTimer("Move time");
            Resource movedResource(move(largeResource));
        }
    }
    
    // Part 3: Assignment operators
    cout << "\n3. Assignment Operators:" << endl;
    {
        Resource r1("Resource1", 100);
        Resource r2("Resource2", 200);
        
        cout << "\nCopy assignment:" << endl;
        Resource r3;
        r3 = r1;  // Copy assignment
        
        cout << "\nMove assignment:" << endl;
        Resource r4;
        r4 = move(r2);  // Move assignment
        
        cout << "\nState after assignments:" << endl;
        cout << "r1 name: " << r1.getName() << ", Size: " << r1.getSize() << endl;
        cout << "r2 name: " << r2.getName() << ", Size: " << r2.getSize() << endl;
        cout << "r3 name: " << r3.getName() << ", Size: " << r3.getSize() << endl;
        cout << "r4 name: " << r4.getName() << ", Size: " << r4.getSize() << endl;
    }
    
    // Part 4: lvalue and rvalue references
    cout << "\n4. lvalue and rvalue References:" << endl;
    {
        Resource resource("LvalueResource", 100);
        
        cout << "\nCalling processByLvalueRef with lvalue:" << endl;
        processByLvalueRef(resource);
        
        cout << "\nCalling processByRvalueRef with rvalue:" << endl;
        processByRvalueRef(move(resource));
        
        cout << "\nState after processing:" << endl;
        cout << "Resource name: " << resource.getName() << ", Size: " << resource.getSize() << endl;
    }
    
    // Part 5: Perfect forwarding
    cout << "\n5. Perfect Forwarding:" << endl;
    {
        Resource r("ForwardedResource", 50);
        
        cout << "\nForwarding lvalue:" << endl;
        perfectForward(r);
        
        cout << "\nForwarding rvalue:" << endl;
        perfectForward(move(r));
        
        cout << "\nCreating resource with perfect forwarding:" << endl;
        Resource r2 = createResource("PerfectResource", 75);
        cout << "Created resource: " << r2.getName() << ", Size: " << r2.getSize() << endl;
    }
    
    // Part 6: Return Value Optimization (RVO)
    cout << "\n6. Return Value Optimization (RVO):" << endl;
    {
        cout << "\nDirect initialization (should use RVO):" << endl;
        Resource rvo = createAndReturnResource("RVOResource", 30);
        cout << "RVO Resource: " << rvo.getName() << ", Size: " << rvo.getSize() << endl;
    }
    
    // Part 7: Move-only types
    cout << "\n7. Move-Only Types:" << endl;
    {
        // Create a move-only object
        MoveOnly mo1;
        mo1.setValue(42);
        
        cout << "\nMoving a move-only object:" << endl;
        MoveOnly mo2 = move(mo1);
        cout << "mo1 value: " << mo1.getValue() << endl;
        cout << "mo2 value: " << mo2.getValue() << endl;
        
        // This would not compile:
        // MoveOnly mo3 = mo2; // Error: copy constructor is deleted
        
        // Put move-only objects in a vector
        cout << "\nStoring move-only objects in a vector:" << endl;
        vector<MoveOnly> moveOnlyVector;
        moveOnlyVector.push_back(move(mo2));
        
        MoveOnly mo3;
        mo3.setValue(100);
        moveOnlyVector.push_back(move(mo3));
        
        cout << "Vector size: " << moveOnlyVector.size() << endl;
    }
    
    // Part 8: String move semantics
    cout << "\n8. String Move Semantics:" << endl;
    {
        // Create a large string
        string largeString(1000000, 'A');
        
        cout << "\nString size before move: " << largeString.size() << endl;
        
        // Create a wrapper with string copy
        StringWrapper wrapper1(largeString);
        cout << "Original string size after copy: " << largeString.size() << endl;
        
        // Create a wrapper with string move
        StringWrapper wrapper2(move(largeString));
        cout << "Original string size after move: " << largeString.size() << endl;
        
        // Show wrapper contents
        cout << "wrapper1 string size: " << wrapper1.getString().size() << endl;
        cout << "wrapper2 string size: " << wrapper2.getString().size() << endl;
    }
    
    cout << "\nEnd of program" << endl;
    return 0;
}