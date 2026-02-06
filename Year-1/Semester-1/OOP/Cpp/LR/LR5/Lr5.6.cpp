#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

// Resource class to demonstrate RAII pattern
class Resource {
private:
    string name;
    
public:
    // Constructor
    Resource(const string& n) : name(n) {
        cout << "Resource " << name << " acquired" << endl;
    }
    
    // Destructor
    ~Resource() {
        cout << "Resource " << name << " released" << endl;
    }
    
    // Use the resource
    void use() const {
        cout << "Using resource " << name << endl;
    }
    
    // Get resource name
    string getName() const {
        return name;
    }
};

// Class with manual memory management
class ManualMemory {
private:
    Resource* resource;
    
public:
    // Constructor
    ManualMemory(const string& name) {
        cout << "ManualMemory constructor called" << endl;
        resource = new Resource(name);
    }
    
    // Destructor
    ~ManualMemory() {
        cout << "ManualMemory destructor called" << endl;
        delete resource;
    }
    
    // Use the resource
    void useResource() const {
        resource->use();
    }
};

// Function that might throw an exception
void riskyFunction(bool shouldThrow) {
    Resource* resource = new Resource("Temporary");
    
    cout << "Doing some work with " << resource->getName() << endl;
    
    if (shouldThrow) {
        cout << "About to throw an exception..." << endl;
        // Memory leak: resource is not deleted if an exception is thrown
        throw runtime_error("Exception in riskyFunction");
    }
    
    delete resource; // Only reached if no exception is thrown
    cout << "Completed work successfully" << endl;
}

// Safer function using smart pointers
void safeFunction(bool shouldThrow) {
    // unique_ptr automatically deletes the resource when it goes out of scope
    unique_ptr<Resource> resource(new Resource("Safe Temporary"));
    
    cout << "Doing some work with " << resource->getName() << endl;
    
    if (shouldThrow) {
        cout << "About to throw an exception..." << endl;
        // No memory leak: unique_ptr's destructor will be called
        throw runtime_error("Exception in safeFunction");
    }
    
    cout << "Completed work successfully" << endl;
    // No need for manual delete - resource is automatically cleaned up
}

int main() {
    cout << "===== Smart Pointers and Memory Management Demo =====" << endl;
    
    // Part 1: Manual memory management
    cout << "\n1. Manual memory management:" << endl;
    {
        ManualMemory manual("Manual");
        manual.useResource();
    } // Destructor called here
    
    // Part 2: Demonstrating potential memory leak with exception
    cout << "\n2. Potential memory leak with exception:" << endl;
    try {
        riskyFunction(false); // No exception, memory is freed
    }
    catch (exception& e) {
        cout << "Caught exception: " << e.what() << endl;
    }
    
    try {
        riskyFunction(true); // Exception occurs, memory leak!
    }
    catch (exception& e) {
        cout << "Caught exception: " << e.what() << endl;
    }
    
    // Part 3: Using unique_ptr for exclusive ownership
    cout << "\n3. Using unique_ptr:" << endl;
    {
        // Create a unique_ptr
        unique_ptr<Resource> resourcePtr(new Resource("Unique"));
        
        // Use the resource
        resourcePtr->use();
        
        // No need to call delete
    } // resourcePtr is automatically destroyed here
    
    // Part 4: Safer exception handling with unique_ptr
    cout << "\n4. Exception safety with unique_ptr:" << endl;
    try {
        safeFunction(false); // No exception
    }
    catch (exception& e) {
        cout << "Caught exception: " << e.what() << endl;
    }
    
    try {
        safeFunction(true); // Exception, but no memory leak
    }
    catch (exception& e) {
        cout << "Caught exception: " << e.what() << endl;
    }
    
    // Part 5: Using shared_ptr for shared ownership
    cout << "\n5. Using shared_ptr:" << endl;
    {
        // Create a shared_ptr
        shared_ptr<Resource> sharedResource1(new Resource("Shared"));
        
        // Use the resource
        sharedResource1->use();
        
        cout << "Reference count: " << sharedResource1.use_count() << endl;
        
        {
            // Create another shared_ptr pointing to the same resource
            shared_ptr<Resource> sharedResource2 = sharedResource1;
            cout << "Reference count after sharing: " << sharedResource1.use_count() << endl;
            
            // Use through the second pointer
            sharedResource2->use();
        } // sharedResource2 is destroyed, but resource is not
        
        cout << "Reference count after inner scope: " << sharedResource1.use_count() << endl;
    } // sharedResource1 is destroyed, and the resource is deleted
    
    // Part 6: Weak pointers
    cout << "\n6. Using weak_ptr to avoid circular references:" << endl;
    {
        // Create a shared_ptr
        shared_ptr<Resource> sharedResource(new Resource("WeakDemo"));
        
        // Create a weak_ptr
        weak_ptr<Resource> weakResource = sharedResource;
        
        cout << "shared_ptr count: " << sharedResource.use_count() << endl;
        
        // Use the weak_ptr
        if (auto tempShared = weakResource.lock()) {
            cout << "Resource is still alive" << endl;
            tempShared->use();
        }
        else {
            cout << "Resource is no longer available" << endl;
        }
        
        // Reset the shared_ptr
        sharedResource.reset();
        cout << "After resetting shared_ptr" << endl;
        
        // Try to use the weak_ptr again
        if (auto tempShared = weakResource.lock()) {
            cout << "Resource is still alive" << endl;
            tempShared->use();
        }
        else {
            cout << "Resource is no longer available" << endl;
        }
    }
    
    // Part 7: Using smart pointers with containers
    cout << "\n7. Using smart pointers with containers:" << endl;
    {
        // Vector of unique_ptr
        vector<unique_ptr<Resource>> resources;
        
        cout << "Adding resources to vector..." << endl;
        resources.push_back(make_unique<Resource>("Vector1"));
        resources.push_back(make_unique<Resource>("Vector2"));
        resources.push_back(make_unique<Resource>("Vector3"));
        
        cout << "Using resources in vector:" << endl;
        for (const auto& res : resources) {
            res->use();
        }
        
        cout << "Vector going out of scope..." << endl;
    } // All resources automatically cleaned up
    
    cout << "\nEnd of program" << endl;
    return 0;
}