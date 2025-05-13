#include <iostream>
using namespace std;

// Абстрактний базовий клас DataStruct
class DataStruct {
protected:
    int *data;     // Масив для зберігання даних
    int size;      // Максимальний розмір структури даних
    int count;     // Поточна кількість елементів
    
public:
    // Конструктор
    DataStruct(int s) {
        size = s > 0 ? s : 10; // Мінімальний розмір 10
        data = new int[size];
        count = 0;
    }
    
    // Віртуальний деструктор для правильного звільнення пам'яті
    virtual ~DataStruct() {
        delete[] data;
    }
    
    // Абстрактні методи, які повинні бути реалізовані в похідних класах
    virtual void add(int item) = 0;  // Додати елемент
    virtual int remove() = 0;        // Видалити та повернути елемент
    virtual bool isEmpty() = 0;      // Перевірка чи структура порожня
    virtual bool isFull() = 0;       // Перевірка чи структура заповнена
    virtual void display() = 0;      // Відображення вмісту
    
    // Неабстрактний метод для отримання кількості елементів
    int getCount() {
        return count;
    }
};

// Похідний клас Stack (Стек) - працює за принципом LIFO (Last In First Out)
class Stack : public DataStruct {
public:
    Stack(int s) : DataStruct(s) {}
    
    // Додавання елемента на вершину стеку
    void add(int item) override {
        if (isFull()) {
            cout << "Stack overflow! Cannot add element: " << item << endl;
            return;
        }
        data[count++] = item;
        cout << "Added " << item << " to stack" << endl;
    }
    
    // Видалення елемента з вершини стеку
    int remove() override {
        if (isEmpty()) {
            cout << "Stack underflow! Cannot remove element from empty stack." << endl;
            return -1; // Повертаємо -1 як ознаку помилки
        }
        int item = data[--count];
        cout << "Removed " << item << " from stack" << endl;
        return item;
    }
    
    // Перевірка чи стек порожній
    bool isEmpty() override {
        return count == 0;
    }
    
    // Перевірка чи стек заповнений
    bool isFull() override {
        return count == size;
    }
    
    // Відображення елементів стеку
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

// Похідний клас Queue (Черга) - працює за принципом FIFO (First In First Out)
class Queue : public DataStruct {
private:
    int front;  // Індекс першого елемента (голова черги)
    int rear;   // Індекс останнього елемента (хвіст черги)
    
public:
    Queue(int s) : DataStruct(s) {
        front = 0;
        rear = -1;
    }
    
    // Додавання елемента в кінець черги
    void add(int item) override {
        if (isFull()) {
            cout << "Queue overflow! Cannot add element: " << item << endl;
            return;
        }
        
        // Циклічне збільшення rear для кругового буфера
        rear = (rear + 1) % size;
        data[rear] = item;
        count++;
        
        cout << "Added " << item << " to queue" << endl;
    }
    
    // Видалення елемента з початку черги
    int remove() override {
        if (isEmpty()) {
            cout << "Queue underflow! Cannot remove element from empty queue." << endl;
            return -1; // Повертаємо -1 як ознаку помилки
        }
        
        int item = data[front];
        
        // Циклічне збільшення front для кругового буфера
        front = (front + 1) % size;
        count--;
        
        cout << "Removed " << item << " from queue" << endl;
        return item;
    }
    
    // Перевірка чи черга порожня
    bool isEmpty() override {
        return count == 0;
    }
    
    // Перевірка чи черга заповнена
    bool isFull() override {
        return count == size;
    }
    
    // Відображення елементів черги
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

// Функція фабрики для створення об'єктів DataStruct
DataStruct *DataStructFactory(char what) {
    const int DEFAULT_SIZE = 5; // Розмір за замовчуванням для демонстраційних цілей
    
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

// Демонстраційна функція для тестування стеку
void demoStack() {
    cout << "\n=== STACK DEMONSTRATION ===\n";
    
    DataStruct *stack = DataStructFactory('s');
    
    // Додавання елементів
    for (int i = 1; i <= 6; i++) {
        stack->add(i * 10);
    }
    
    // Відображення стеку
    stack->display();
    
    // Видалення елементів
    for (int i = 0; i < 3; i++) {
        stack->remove();
    }
    
    // Відображення після видалення
    stack->display();
    
    // Додавання після видалення
    stack->add(100);
    stack->add(200);
    
    // Фінальне відображення
    stack->display();
    
    delete stack;
}

// Демонстраційна функція для тестування черги
void demoQueue() {
    cout << "\n=== QUEUE DEMONSTRATION ===\n";
    
    DataStruct *queue = DataStructFactory('q');
    
    // Додавання елементів
    for (int i = 1; i <= 6; i++) {
        queue->add(i * 10);
    }
    
    // Відображення черги
    queue->display();
    
    // Видалення елементів
    for (int i = 0; i < 3; i++) {
        queue->remove();
    }
    
    // Відображення після видалення
    queue->display();
    
    // Додавання після видалення
    queue->add(100);
    queue->add(200);
    
    // Фінальне відображення
    queue->display();
    
    delete queue;
}

// Інтерактивна демонстрація з вибором користувача
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
    
    // Автоматична демонстрація стеку
    demoStack();
    
    // Автоматична демонстрація черги
    demoQueue();
    
    // Інтерактивна демонстрація
    interactiveDemo();
 
    return 0;
}