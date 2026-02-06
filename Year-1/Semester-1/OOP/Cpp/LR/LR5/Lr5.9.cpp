#include <iostream>
#include <string>
using namespace std;

// Base class: Person
class Person {
protected:
    string name;
    int age;
    
public:
    // Constructor
    Person(const string& n = "Unknown", int a = 0) : name(n), age(a) {
        cout << "Person constructor called for " << name << endl;
    }
    
    // Destructor
    virtual ~Person() {
        cout << "Person destructor called for " << name << endl;
    }
    
    // Display info
    virtual void display() const {
        cout << "Name: " << name << ", Age: " << age << endl;
    }
    
    // Getters and setters
    string getName() const { return name; }
    void setName(const string& n) { name = n; }
    
    int getAge() const { return age; }
    void setAge(int a) { age = a; }
};

// Base class: Employee (inherits from Person)
class Employee : virtual public Person {
protected:
    string employeeId;
    double salary;
    
public:
    // Constructor
    Employee(const string& n = "Unknown", int a = 0, 
             const string& id = "E000", double s = 0.0)
        : Person(n, a), employeeId(id), salary(s) {
        cout << "Employee constructor called for " << name << endl;
    }
    
    // Destructor
    ~Employee() {
        cout << "Employee destructor called for " << name << endl;
    }
    
    // Display info
    void display() const override {
        Person::display();
        cout << "Employee ID: " << employeeId << ", Salary: $" << salary << endl;
    }
    
    // Work function
    virtual void work() const {
        cout << name << " is working as an employee." << endl;
    }
    
    // Getters and setters
    string getEmployeeId() const { return employeeId; }
    void setEmployeeId(const string& id) { employeeId = id; }
    
    double getSalary() const { return salary; }
    void setSalary(double s) { salary = s; }
};

// Base class: Student (inherits from Person)
class Student : virtual public Person {
protected:
    string studentId;
    double gpa;
    
public:
    // Constructor
    Student(const string& n = "Unknown", int a = 0,
            const string& id = "S000", double g = 0.0)
        : Person(n, a), studentId(id), gpa(g) {
        cout << "Student constructor called for " << name << endl;
    }
    
    // Destructor
    ~Student() {
        cout << "Student destructor called for " << name << endl;
    }
    
    // Display info
    void display() const override {
        Person::display();
        cout << "Student ID: " << studentId << ", GPA: " << gpa << endl;
    }
    
    // Study function
    virtual void study() const {
        cout << name << " is studying." << endl;
    }
    
    // Getters and setters
    string getStudentId() const { return studentId; }
    void setStudentId(const string& id) { studentId = id; }
    
    double getGpa() const { return gpa; }
    void setGpa(double g) { gpa = g; }
};

// Derived class: TeachingAssistant (multiple inheritance)
class TeachingAssistant : public Employee, public Student {
private:
    string department;
    int hoursPerWeek;
    
public:
    // Constructor
    TeachingAssistant(const string& n = "Unknown", int a = 0,
                      const string& eId = "E000", double s = 0.0,
                      const string& sId = "S000", double g = 0.0,
                      const string& dept = "Unknown", int hours = 0)
        : Person(n, a), // Call the Person constructor directly
          Employee(n, a, eId, s),
          Student(n, a, sId, g),
          department(dept), hoursPerWeek(hours) {
        cout << "TeachingAssistant constructor called for " << name << endl;
    }
    
    // Destructor
    ~TeachingAssistant() {
        cout << "TeachingAssistant destructor called for " << name << endl;
    }
    
    // Display info
    void display() const override {
        Person::display(); // Call Person's display directly
        cout << "Employee ID: " << employeeId << ", Salary: $" << salary << endl;
        cout << "Student ID: " << studentId << ", GPA: " << gpa << endl;
        cout << "Department: " << department << ", Hours/Week: " << hoursPerWeek << endl;
    }
    
    // Work function (override Employee's work)
    void work() const override {
        cout << name << " is working as a teaching assistant in the " 
             << department << " department." << endl;
    }
    
    // Study function (override Student's study)
    void study() const override {
        cout << name << " is studying advanced topics in the " 
             << department << " department." << endl;
    }
    
    // Teach function (specific to TeachingAssistant)
    void teach() const {
        cout << name << " is teaching a class in the " 
             << department << " department." << endl;
    }
    
    // Hold office hours function (specific to TeachingAssistant)
    void holdOfficeHours() const {
        cout << name << " is holding office hours for " 
             << hoursPerWeek << " hours per week." << endl;
    }
    
    // Getters and setters
    string getDepartment() const { return department; }
    void setDepartment(const string& dept) { department = dept; }
    
    int getHoursPerWeek() const { return hoursPerWeek; }
    void setHoursPerWeek(int hours) { hoursPerWeek = hours; }
};

// Demonstrate the diamond problem with non-virtual inheritance
class BaseWithData {
protected:
    int data;
    
public:
    // Constructor
    BaseWithData(int d = 0) : data(d) {
        cout << "BaseWithData constructor called, data = " << data << endl;
    }
    
    // Destructor
    virtual ~BaseWithData() {
        cout << "BaseWithData destructor called, data = " << data << endl;
    }
    
    // Display data
    virtual void display() const {
        cout << "BaseWithData::data = " << data << endl;
    }
    
    // Getter and setter
    int getData() const { return data; }
    void setData(int d) { data = d; }
};

// Non-virtual inheritance
class Derived1 : public BaseWithData {
public:
    // Constructor
    Derived1(int d = 10) : BaseWithData(d) {
        cout << "Derived1 constructor called" << endl;
    }
    
    // Destructor
    ~Derived1() {
        cout << "Derived1 destructor called" << endl;
    }
};

// Non-virtual inheritance
class Derived2 : public BaseWithData {
public:
    // Constructor
    Derived2(int d = 20) : BaseWithData(d) {
        cout << "Derived2 constructor called" << endl;
    }
    
    // Destructor
    ~Derived2() {
        cout << "Derived2 destructor called" << endl;
    }
};

// Diamond inheritance without virtual inheritance
class DiamondWithoutVirtual : public Derived1, public Derived2 {
public:
    // Constructor
    DiamondWithoutVirtual() {
        cout << "DiamondWithoutVirtual constructor called" << endl;
    }
    
    // Destructor
    ~DiamondWithoutVirtual() {
        cout << "DiamondWithoutVirtual destructor called" << endl;
    }
    
    // Display data (now ambiguous which data to display)
    void display() const {
        cout << "DiamondWithoutVirtual: Derived1::data = " << Derived1::data << endl;
        cout << "DiamondWithoutVirtual: Derived2::data = " << Derived2::data << endl;
    }
};

// Virtual inheritance
class VDerived1 : virtual public BaseWithData {
public:
    // Constructor
    VDerived1(int d = 30) : BaseWithData(d) {
        cout << "VDerived1 constructor called" << endl;
    }
    
    // Destructor
    ~VDerived1() {
        cout << "VDerived1 destructor called" << endl;
    }
};

// Virtual inheritance
class VDerived2 : virtual public BaseWithData {
public:
    // Constructor
    VDerived2(int d = 40) : BaseWithData(d) {
        cout << "VDerived2 constructor called" << endl;
    }
    
    // Destructor
    ~VDerived2() {
        cout << "VDerived2 destructor called" << endl;
    }
};

// Diamond inheritance with virtual inheritance
class DiamondWithVirtual : public VDerived1, public VDerived2 {
public:
    // Constructor
    DiamondWithVirtual() : BaseWithData(50) { // Initialize the shared base directly
        cout << "DiamondWithVirtual constructor called" << endl;
    }
    
    // Destructor
    ~DiamondWithVirtual() {
        cout << "DiamondWithVirtual destructor called" << endl;
    }
    
    // Display data (no ambiguity now)
    void display() const override {
        cout << "DiamondWithVirtual: BaseWithData::data = " << data << endl;
    }
};

int main() {
    cout << "===== Multiple Inheritance and Virtual Inheritance Demo =====" << endl;
    
    // Part 1: TeachingAssistant with virtual inheritance
    cout << "\n1. TeachingAssistant Class Demonstration:" << endl;
    {
        cout << "\nCreating a TeachingAssistant object:" << endl;
        TeachingAssistant ta(
            "Alice Thompson", 28,    // Person attributes
            "E12345", 25000.0,       // Employee attributes
            "S67890", 3.85,          // Student attributes
            "Computer Science", 15    // TA attributes
        );
        
        cout << "\nTeachingAssistant details:" << endl;
        ta.display();
        
        cout << "\nCalling TeachingAssistant methods:" << endl;
        ta.work();
        ta.study();
        ta.teach();
        ta.holdOfficeHours();
        
        cout << "\nAccessing attributes through different interfaces:" << endl;
        cout << "Name (through Person): " << ta.getName() << endl;
        cout << "Employee ID: " << ta.getEmployeeId() << endl;
        cout << "Student ID: " << ta.getStudentId() << endl;
        cout << "Department: " << ta.getDepartment() << endl;
        
        cout << "\nTeachingAssistant object going out of scope..." << endl;
    }
    
    // Part 2: Diamond problem without virtual inheritance
    cout << "\n2. Diamond Problem Without Virtual Inheritance:" << endl;
    {
        cout << "\nCreating a DiamondWithoutVirtual object:" << endl;
        DiamondWithoutVirtual diamond1;
        
        cout << "\nSetting data in Derived1:" << endl;
        diamond1.Derived1::setData(15);
        
        cout << "\nSetting data in Derived2:" << endl;
        diamond1.Derived2::setData(25);
        
        cout << "\nDisplaying data:" << endl;
        diamond1.display();
        
        // Need to specify which base's method to call (ambiguous)
        cout << "\nAccessing data through specific paths:" << endl;
        cout << "Derived1::getData() = " << diamond1.Derived1::getData() << endl;
        cout << "Derived2::getData() = " << diamond1.Derived2::getData() << endl;
        
        cout << "\nDiamondWithoutVirtual object going out of scope..." << endl;
    }
    
    // Part 3: Diamond problem with virtual inheritance
    cout << "\n3. Diamond Problem With Virtual Inheritance:" << endl;
    {
        cout << "\nCreating a DiamondWithVirtual object:" << endl;
        DiamondWithVirtual diamond2;
        
        cout << "\nSetting data in the shared base:" << endl;
        diamond2.setData(55);
        
        cout << "\nDisplaying data:" << endl;
        diamond2.display();
        
        // No ambiguity now - there's only one instance of BaseWithData
        cout << "\nAccessing data directly:" << endl;
        cout << "getData() = " << diamond2.getData() << endl;
        
        cout << "\nDiamondWithVirtual object going out of scope..." << endl;
    }
    
    cout << "\nEnd of program" << endl;
    return 0;
}