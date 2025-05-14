#include <iostream>
using namespace std;

class DataStruct {
protected:
    int *data;    
    int size;   
    int count;     
    
public:
    DataStruct(int s) {
        size = s > 0 ? s : 10; 
        data = new int[size];
        count = 0;
    }
    
    virtual ~DataStruct() {
        delete[] data;
    }
    
    virtual void add(int item) = 0; 
    virtual int remove() = 0;      
    virtual bool isEmpty() = 0;    
    virtual bool isFull() = 0; 
    virtual void display() = 0;
    
    int getCount() {
        return count;
    }
};

class Stack : public DataStruct {
public:
    Stack(int s) : DataStruct(s) {}
    
    void add(int item) override {
        if (isFull()) {
            cout << "Stack overflow! Cannot add element: " << item << endl;
            return;
        }
        data[count++] = item;
        cout << "Added " << item << " to stack" << endl;
    }
    
    int remove() override {
        if (isEmpty()) {
            cout << "Stack underflow! Cannot remove element from empty stack." << endl;
            return -1;
        }
        int item = data[--count];
        cout << "Removed " << item << " from stack" << endl;
        return item;
    }
    
    bool isEmpty() override {
        return count == 0;
    }
    
    bool isFull() override {
        return count == size;
    }
    
    void display() override {
        if (isEmpty()) {
            cout << "Stack is empty" << endl;
            return;
        }
        
        cout << "Stack contents (top to bottom): ";
        for (int i = count - 1; i >= 0; i--) {
            cout << data[i];
            if (i > 0) cout << " ";
        }
        cout << endl;
    }
};

class Queue : public DataStruct {
private:
    int front;
    int rear;
    
public:
    Queue(int s) : DataStruct(s) {
        front = 0;
        rear = -1;
    }
    
    void add(int item) override {
        if (isFull()) {
            cout << "Queue overflow! Cannot add element: " << item << endl;
            return;
        }
        
        rear = (rear + 1) % size;
        data[rear] = item;
        count++;
        
        cout << "Added " << item << " to queue" << endl;
    }
    
    int remove() override {
        if (isEmpty()) {
            cout << "Queue underflow! Cannot remove element from empty queue." << endl;
            return -1;
        }
        
        int item = data[front];
        
        front = (front + 1) % size;
        count--;
        
        cout << "Removed " << item << " from queue" << endl;
        return item;
    }
    
    bool isEmpty() override {
        return count == 0;
    }
    
    bool isFull() override {
        return count == size;
    }
    
    void display() override {
        if (isEmpty()) {
            cout << "Queue is empty" << endl;
            return;
        }
        
        cout << "Queue contents (front to rear): ";
        
        int current = front;
        for (int i = 0; i < count; i++) {
            cout << data[current];
            if (i < count - 1) cout << " ";
            current = (current + 1) % size;
        }
        cout << endl;
    }
};

DataStruct *DataStructFactory(char what) {
    const int DEFAULT_SIZE = 5;
    
    if (what == 's' || what == 'S') {
        cout << "Creating Stack with size " << DEFAULT_SIZE << endl;
        return new Stack(DEFAULT_SIZE);
    }
    else if (what == 'q' || what == 'Q') {
        cout << "Creating Queue with size " << DEFAULT_SIZE << endl;
        return new Queue(DEFAULT_SIZE);
    }
    
    cout << "Invalid option. Available options: 's' for Stack, 'q' for Queue" << endl;
    return nullptr;
}

void demoStack() {
    cout << "\n=== STACK DEMONSTRATION ===\n";
    
    DataStruct *stack = DataStructFactory('s');
    
    for (int i = 1; i <= 6; i++) {
        stack->add(i * 10);
    }
    
    stack->display();
    
    for (int i = 0; i < 3; i++) {
        stack->remove();
    }
    
    stack->display();
    
    stack->add(100);
    stack->add(200);
    
    stack->display();
    
    delete stack;
}

void demoQueue() {
    cout << "\n=== QUEUE DEMONSTRATION ===\n";
    
    DataStruct *queue = DataStructFactory('q');
    
    for (int i = 1; i <= 6; i++) {
        queue->add(i * 10);
    }
    
    queue->display();
    
    for (int i = 0; i < 3; i++) {
        queue->remove();
    }
    
    queue->display();
    
    queue->add(100);
    queue->add(200);
    
    queue->display();
    
    delete queue;
}

void interactiveDemo() {
    char choice;
    
    cout << "\n=== INTERACTIVE DEMONSTRATION ===\n";
    cout << "Enter 's' for Stack or 'q' for Queue: ";
    cin >> choice;
    
    DataStruct *ds = DataStructFactory(choice);
    
    if (ds) {
        int option;
        int value;
        
        do {
            cout << "\nOptions:\n";
            cout << "1. Add element\n";
            cout << "2. Remove element\n";
            cout << "3. Display\n";
            cout << "4. Exit\n";
            cout << "Enter your choice: ";
            cin >> option;
            
            switch (option) {
                case 1:
                    cout << "Enter value to add: ";
                    cin >> value;
                    ds->add(value);
                    break;
                case 2:
                    ds->remove();
                    break;
                case 3:
                    ds->display();
                    break;
                case 4:
                    cout << "Exiting...\n";
                    break;
                default:
                    cout << "Invalid option!\n";
            }
        } while (option != 4);
        
        delete ds;
    }
}

int main() {
    cout << "=== DataStruct Hierarchy Demonstration ===\n";
    
    demoStack();
    
    demoQueue();
    
    interactiveDemo();
 
    return 0;
}