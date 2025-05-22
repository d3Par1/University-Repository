#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <functional>
#include <numeric>
using namespace std;

// Sample structure for STL container examples
struct Student {
    int id;
    string name;
    double gpa;
    
    // Constructor
    Student(int i, const string& n, double g) : id(i), name(n), gpa(g) {}
    
    // For comparison and sorting
    bool operator<(const Student& other) const {
        return id < other.id;
    }
    
    // For equality check
    bool operator==(const Student& other) const {
        return id == other.id;
    }
};

// Display a container's elements
template <typename Container>
void displayContainer(const string& label, const Container& container) {
    cout << label << ": ";
    for (const auto& elem : container) {
        cout << elem << " ";
    }
    cout << endl;
}

// Display a map's elements
template <typename Key, typename Value>
void displayMap(const string& label, const map<Key, Value>& container) {
    cout << label << ":" << endl;
    for (const auto& pair : container) {
        cout << "  " << pair.first << " => " << pair.second << endl;
    }
    cout << endl;
}

// Display student information
void displayStudent(const Student& student) {
    cout << "ID: " << student.id << ", Name: " << student.name 
         << ", GPA: " << student.gpa << endl;
}

// Display a vector of students
void displayStudents(const string& label, const vector<Student>& students) {
    cout << label << ":" << endl;
    for (const auto& student : students) {
        cout << "  ";
        displayStudent(student);
    }
    cout << endl;
}

int main() {
    cout << "===== STL Containers and Algorithms Demo =====" << endl;
    
    // Part 1: STL Vector
    cout << "\n1. STL Vector:" << endl;
    {
        // Create a vector
        vector<int> numbers{10, 20, 30, 40, 50};
        
        // Display vector
        displayContainer("Initial vector", numbers);
        
        // Add elements
        numbers.push_back(60);
        numbers.push_back(70);
        displayContainer("After adding elements", numbers);
        
        // Insert element at position
        numbers.insert(numbers.begin() + 3, 35);
        displayContainer("After inserting 35 at position 3", numbers);
        
        // Access elements
        cout << "Element at index 4: " << numbers[4] << endl;
        cout << "Element at index 2 (using at()): " << numbers.at(2) << endl;
        
        // Size and capacity
        cout << "Size: " << numbers.size() << endl;
        cout << "Capacity: " << numbers.capacity() << endl;
        
        // Erase element
        numbers.erase(numbers.begin() + 2);
        displayContainer("After erasing element at position 2", numbers);
        
        // Clear vector
        numbers.clear();
        cout << "After clear(), size: " << numbers.size() << endl;
    }
    
    // Part 2: STL List
    cout << "\n2. STL List:" << endl;
    {
        // Create a list
        list<string> names{"Alice", "Bob", "Charlie", "David"};
        
        // Display list
        displayContainer("Initial list", names);
        
        // Add elements
        names.push_back("Eve");
        names.push_front("Aaron");
        displayContainer("After adding elements", names);
        
        // Insert element
        auto it = find(names.begin(), names.end(), "Charlie");
        if (it != names.end()) {
            names.insert(it, "Carol");
        }
        displayContainer("After inserting Carol before Charlie", names);
        
        // Remove elements
        names.remove("Bob");
        displayContainer("After removing Bob", names);
        
        // Sort list
        names.sort();
        displayContainer("After sorting", names);
        
        // Reverse list
        names.reverse();
        displayContainer("After reversing", names);
    }
    
    // Part 3: STL Set
    cout << "\n3. STL Set:" << endl;
    {
        // Create a set
        set<int> uniqueNumbers{50, 30, 10, 40, 20, 10}; // Note: duplicates are ignored
        
        // Display set (automatically sorted)
        displayContainer("Initial set", uniqueNumbers);
        
        // Insert elements
        uniqueNumbers.insert(5);
        uniqueNumbers.insert(45);
        displayContainer("After inserting elements", uniqueNumbers);
        
        // Check if element exists
        int searchValue = 30;
        if (uniqueNumbers.find(searchValue) != uniqueNumbers.end()) {
            cout << searchValue << " exists in the set" << endl;
        }
        
        // Remove element
        uniqueNumbers.erase(40);
        displayContainer("After removing 40", uniqueNumbers);
        
        // Size
        cout << "Set size: " << uniqueNumbers.size() << endl;
    }
    
    // Part 4: STL Map
    cout << "\n4. STL Map:" << endl;
    {
        // Create a map (key-value pairs)
        map<string, int> ages{
            {"Alice", 25},
            {"Bob", 30},
            {"Charlie", 35}
        };
        
        // Display map
        displayMap("Initial map", ages);
        
        // Insert elements
        ages["David"] = 40;
        ages.insert({"Eve", 28});
        displayMap("After adding elements", ages);
        
        // Access elements
        cout << "Bob's age: " << ages["Bob"] << endl;
        
        // Check if key exists and access
        string name = "Frank";
        if (ages.find(name) != ages.end()) {
            cout << name << "'s age: " << ages[name] << endl;
        }
        else {
            cout << name << " not found in the map" << endl;
        }
        
        // Update value
        ages["Alice"] = 26;
        cout << "Alice's updated age: " << ages["Alice"] << endl;
        
        // Remove element
        ages.erase("Charlie");
        displayMap("After removing Charlie", ages);
        
        // Size
        cout << "Map size: " << ages.size() << endl;
    }
    
    // Part 5: STL Algorithms
    cout << "\n5. STL Algorithms:" << endl;
    {
        // Create a vector
        vector<int> data{9, 5, 3, 7, 1, 8, 2, 6, 4};
        displayContainer("Original vector", data);
        
        // Sort algorithm
        sort(data.begin(), data.end());
        displayContainer("After sorting", data);
        
        // Reverse algorithm
        reverse(data.begin(), data.end());
        displayContainer("After reversing", data);
        
        // Find algorithm
        int searchValue = 7;
        auto it = find(data.begin(), data.end(), searchValue);
        if (it != data.end()) {
            cout << "Found " << searchValue << " at position: " 
                 << distance(data.begin(), it) << endl;
        }
        
        // Count algorithm
        vector<int> values{1, 2, 3, 4, 2, 5, 2, 6};
        int countValue = 2;
        int occurrences = count(values.begin(), values.end(), countValue);
        cout << countValue << " appears " << occurrences << " times" << endl;
        
        // Min and max elements
        auto minIt = min_element(data.begin(), data.end());
        auto maxIt = max_element(data.begin(), data.end());
        cout << "Min element: " << *minIt << endl;
        cout << "Max element: " << *maxIt << endl;
        
        // Accumulate algorithm
        int sum = accumulate(data.begin(), data.end(), 0);
        cout << "Sum of elements: " << sum << endl;
        
        // Transform algorithm
        vector<int> squared(data.size());
        transform(data.begin(), data.end(), squared.begin(),
                 [](int x) { return x * x; });
        displayContainer("Squared values", squared);
        
        // For_each algorithm
        cout << "Printing elements with for_each: ";
        for_each(data.begin(), data.end(), 
                [](int x) { cout << x << " "; });
        cout << endl;
    }
    
    // Part 6: Working with custom objects
    cout << "\n6. Working with custom objects:" << endl;
    {
        // Create a vector of students
        vector<Student> students{
            {1001, "Alice", 3.8},
            {1003, "Bob", 3.5},
            {1002, "Charlie", 3.9},
            {1005, "David", 3.2},
            {1004, "Eve", 3.7}
        };
        
        displayStudents("Original students", students);
        
        // Sort by ID (using Student's operator<)
        sort(students.begin(), students.end());
        displayStudents("Sorted by ID", students);
        
        // Sort by GPA (using custom comparator)
        sort(students.begin(), students.end(),
            [](const Student& a, const Student& b) {
                return a.gpa > b.gpa; // Sort in descending order
            });
        displayStudents("Sorted by GPA (descending)", students);
        
        // Sort by name
        sort(students.begin(), students.end(),
            [](const Student& a, const Student& b) {
                return a.name < b.name;
            });
        displayStudents("Sorted by name", students);
        
        // Find a student by ID
        int searchId = 1003;
        auto it = find_if(students.begin(), students.end(),
                         [searchId](const Student& s) { return s.id == searchId; });
        if (it != students.end()) {
            cout << "Found student with ID " << searchId << ": " << it->name << endl;
        }
        
        // Filter students with GPA >= 3.7
        vector<Student> highGpaStudents;
        copy_if(students.begin(), students.end(), back_inserter(highGpaStudents),
               [](const Student& s) { return s.gpa >= 3.7; });
        displayStudents("Students with GPA >= 3.7", highGpaStudents);
        
        // Calculate average GPA
        double totalGpa = accumulate(students.begin(), students.end(), 0.0,
                                    [](double sum, const Student& s) { return sum + s.gpa; });
        double averageGpa = totalGpa / students.size();
        cout << "Average GPA: " << averageGpa << endl;
    }
    
    cout << "\nEnd of program" << endl;
    return 0;
}