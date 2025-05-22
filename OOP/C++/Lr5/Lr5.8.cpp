#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

// Student structure for file operations
struct Student {
    int id;
    string firstName;
    string lastName;
    double gpa;
    
    // Default constructor
    Student() : id(0), firstName(""), lastName(""), gpa(0.0) {}
    
    // Parameterized constructor
    Student(int i, const string& first, const string& last, double g)
        : id(i), firstName(first), lastName(last), gpa(g) {}
    
    // Display student information
    void display() const {
        cout << setw(5) << id << " | "
             << setw(10) << firstName << " | "
             << setw(10) << lastName << " | "
             << fixed << setprecision(2) << gpa << endl;
    }
};

// Function to save students to a text file
bool saveStudentsToText(const vector<Student>& students, const string& filename) {
    ofstream outFile(filename);
    
    if (!outFile) {
        cerr << "Error opening file for writing: " << filename << endl;
        return false;
    }
    
    // Write header
    outFile << "ID,FirstName,LastName,GPA" << endl;
    
    // Write each student
    for (const auto& student : students) {
        outFile << student.id << ","
                << student.firstName << ","
                << student.lastName << ","
                << fixed << setprecision(2) << student.gpa << endl;
    }
    
    outFile.close();
    return true;
}

// Function to load students from a text file
bool loadStudentsFromText(vector<Student>& students, const string& filename) {
    ifstream inFile(filename);
    
    if (!inFile) {
        cerr << "Error opening file for reading: " << filename << endl;
        return false;
    }
    
    students.clear();
    string line;
    
    // Skip header
    getline(inFile, line);
    
    // Read each line
    while (getline(inFile, line)) {
        stringstream ss(line);
        string idStr, firstName, lastName, gpaStr;
        
        // Parse CSV format
        getline(ss, idStr, ',');
        getline(ss, firstName, ',');
        getline(ss, lastName, ',');
        getline(ss, gpaStr, ',');
        
        try {
            int id = stoi(idStr);
            double gpa = stod(gpaStr);
            students.emplace_back(id, firstName, lastName, gpa);
        }
        catch (const exception& e) {
            cerr << "Error parsing line: " << line << endl;
            cerr << "Error message: " << e.what() << endl;
        }
    }
    
    inFile.close();
    return true;
}

// Function to save students to a binary file
bool saveStudentsToBinary(const vector<Student>& students, const string& filename) {
    ofstream outFile(filename, ios::binary | ios::out);
    
    if (!outFile) {
        cerr << "Error opening binary file for writing: " << filename << endl;
        return false;
    }
    
    // Write number of students
    int numStudents = students.size();
    outFile.write(reinterpret_cast<const char*>(&numStudents), sizeof(numStudents));
    
    // Write each student
    for (const auto& student : students) {
        // Write ID
        outFile.write(reinterpret_cast<const char*>(&student.id), sizeof(student.id));
        
        // Write first name length and content
        int firstNameLength = student.firstName.length();
        outFile.write(reinterpret_cast<const char*>(&firstNameLength), sizeof(firstNameLength));
        outFile.write(student.firstName.c_str(), firstNameLength);
        
        // Write last name length and content
        int lastNameLength = student.lastName.length();
        outFile.write(reinterpret_cast<const char*>(&lastNameLength), sizeof(lastNameLength));
        outFile.write(student.lastName.c_str(), lastNameLength);
        
        // Write GPA
        outFile.write(reinterpret_cast<const char*>(&student.gpa), sizeof(student.gpa));
    }
    
    outFile.close();
    return true;
}

// Function to load students from a binary file
bool loadStudentsFromBinary(vector<Student>& students, const string& filename) {
    ifstream inFile(filename, ios::binary | ios::in);
    
    if (!inFile) {
        cerr << "Error opening binary file for reading: " << filename << endl;
        return false;
    }
    
    students.clear();
    
    // Read number of students
    int numStudents;
    inFile.read(reinterpret_cast<char*>(&numStudents), sizeof(numStudents));
    
    // Read each student
    for (int i = 0; i < numStudents; i++) {
        Student student;
        
        // Read ID
        inFile.read(reinterpret_cast<char*>(&student.id), sizeof(student.id));
        
        // Read first name length and content
        int firstNameLength;
        inFile.read(reinterpret_cast<char*>(&firstNameLength), sizeof(firstNameLength));
        
        char* firstNameBuffer = new char[firstNameLength + 1];
        inFile.read(firstNameBuffer, firstNameLength);
        firstNameBuffer[firstNameLength] = '\0';
        student.firstName = firstNameBuffer;
        delete[] firstNameBuffer;
        
        // Read last name length and content
        int lastNameLength;
        inFile.read(reinterpret_cast<char*>(&lastNameLength), sizeof(lastNameLength));
        
        char* lastNameBuffer = new char[lastNameLength + 1];
        inFile.read(lastNameBuffer, lastNameLength);
        lastNameBuffer[lastNameLength] = '\0';
        student.lastName = lastNameBuffer;
        delete[] lastNameBuffer;
        
        // Read GPA
        inFile.read(reinterpret_cast<char*>(&student.gpa), sizeof(student.gpa));
        
        students.push_back(student);
    }
    
    inFile.close();
    return true;
}

// Function to append data to an existing file
bool appendStudentToFile(const Student& student, const string& filename) {
    ofstream outFile(filename, ios::app);
    
    if (!outFile) {
        cerr << "Error opening file for appending: " << filename << endl;
        return false;
    }
    
    outFile << student.id << ","
            << student.firstName << ","
            << student.lastName << ","
            << fixed << setprecision(2) << student.gpa << endl;
    
    outFile.close();
    return true;
}

// Function to display all students
void displayStudents(const vector<Student>& students) {
    cout << setw(5) << "ID" << " | "
         << setw(10) << "First Name" << " | "
         << setw(10) << "Last Name" << " | "
         << "GPA" << endl;
    cout << string(40, '-') << endl;
    
    for (const auto& student : students) {
        student.display();
    }
}

int main() {
    cout << "===== File I/O Operations Demo =====" << endl;
    
    // Create a vector of students
    vector<Student> students = {
        {1001, "John", "Smith", 3.75},
        {1002, "Mary", "Johnson", 3.95},
        {1003, "James", "Williams", 3.50},
        {1004, "Patricia", "Brown", 3.82},
        {1005, "Robert", "Jones", 3.67}
    };
    
    // Part 1: Text file operations
    cout << "\n1. Text File Operations:" << endl;
    {
        const string textFilename = "students.csv";
        
        // Save students to text file
        cout << "Saving students to text file..." << endl;
        if (saveStudentsToText(students, textFilename)) {
            cout << "Successfully saved " << students.size() << " students to " << textFilename << endl;
        }
        
        // Load students from text file
        vector<Student> loadedStudents;
        cout << "Loading students from text file..." << endl;
        if (loadStudentsFromText(loadedStudents, textFilename)) {
            cout << "Successfully loaded " << loadedStudents.size() << " students from " << textFilename << endl;
            
            cout << "\nLoaded students:" << endl;
            displayStudents(loadedStudents);
        }
        
        // Append a new student
        Student newStudent(1006, "Elizabeth", "Davis", 3.91);
        cout << "\nAppending a new student to text file..." << endl;
        if (appendStudentToFile(newStudent, textFilename)) {
            cout << "Successfully appended student to " << textFilename << endl;
            
            // Reload to show the appended student
            vector<Student> updatedStudents;
            if (loadStudentsFromText(updatedStudents, textFilename)) {
                cout << "\nUpdated students list:" << endl;
                displayStudents(updatedStudents);
            }
        }
    }
    
    // Part 2: Binary file operations
    cout << "\n2. Binary File Operations:" << endl;
    {
        const string binaryFilename = "students.bin";
        
        // Save students to binary file
        cout << "Saving students to binary file..." << endl;
        if (saveStudentsToBinary(students, binaryFilename)) {
            cout << "Successfully saved " << students.size() << " students to " << binaryFilename << endl;
        }
        
        // Load students from binary file
        vector<Student> loadedStudents;
        cout << "Loading students from binary file..." << endl;
        if (loadStudentsFromBinary(loadedStudents, binaryFilename)) {
            cout << "Successfully loaded " << loadedStudents.size() << " students from " << binaryFilename << endl;
            
            cout << "\nLoaded students:" << endl;
            displayStudents(loadedStudents);
        }
    }
    
    // Part 3: Stream manipulators
    cout << "\n3. Stream Manipulators Demo:" << endl;
    {
        // Create a stringstream
        stringstream ss;
        
        // Use various manipulators
        ss << fixed << setprecision(2) << 123.456789 << endl;
        ss << scientific << 123.456789 << endl;
        ss << hexfloat << 123.456789 << endl;
        ss << defaultfloat << endl;
        
        ss << setw(10) << setfill('*') << 123 << endl;
        ss << setw(10) << left << setfill('-') << 123 << endl;
        ss << setw(10) << right << setfill(' ') << 123 << endl;
        
        ss << hex << showbase << 255 << endl;
        ss << oct << 255 << endl;
        ss << dec << 255 << endl;
        
        ss << boolalpha << true << " " << false << endl;
        
        // Display stringstream content
        cout << "Stringstream content with various manipulators:" << endl;
        cout << ss.str();
    }
    
    cout << "\nEnd of program" << endl;
    return 0;
}