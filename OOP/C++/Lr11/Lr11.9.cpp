#include <iostream>
using namespace std;

class list {
public:
    list *head;  
    list *tail;  
    list *next;  
    int num;     
    
    list() { head = tail = next = NULL; }
    
    virtual void store(int i) = 0;     
    virtual int retrieve() = 0;        
    
    virtual ~list() {} 
};

class queue : public list {
public:
    queue() { head = tail = NULL; }
    
    void store(int i) override {
        list *item;
        item = new queue;
        if(!item) {
            cout << "Allocation error.\n";
            exit(1);
        }
        item->num = i;
        
        if(tail) tail->next = item;
        tail = item;
        item->next = NULL;
        if(!head) head = tail;
    }
    
    int retrieve() override {
        int i;
        list *p;
        
        if(!head) {
            cout << "List empty.\n";
            return 0;
        }
        
        i = head->num;
        p = head;
        head = head->next;
        delete p;
        return i;
    }
    
    queue& operator+(int value) {
        store(value);
        return *this;
    }
    
    friend int operator-(queue &q) {
        return q.retrieve();
    }
    
    void display() {
        if(!head) {
            cout << "Queue is empty.\n";
            return;
        }
        
        cout << "Queue contents: ";
        list *current = head;
        while(current) {
            cout << current->num << " ";
            current = current->next;
        }
        cout << endl;
    }
};

class stack : public list {
public:
    stack() { head = NULL; }
    
    void store(int i) override {
        list *item;
        item = new stack;
        if(!item) {
            cout << "Allocation error.\n";
            exit(1);
        }
        item->num = i;
        
        // put on front of list
        if(head) item->next = head;
        head = item;
    }
    
    int retrieve() override {
        int i;
        list *p;
        
        if(!head) {
            cout << "List empty.\n";
            return 0;
        }
        
        i = head->num;
        p = head;
        head = head->next;
        delete p;
        return i;
    }
    
    stack& operator+(int value) {
        store(value);
        return *this;
    }
    
    friend int operator-(stack &s) {
        return s.retrieve();
    }
    
    void display() {
        if(!head) {
            cout << "Stack is empty.\n";
            return;
        }
        
        cout << "Stack contents (top to bottom): ";
        list *current = head;
        while(current) {
            cout << current->num << " ";
            current = current->next;
        }
        cout << endl;
    }
};

int main() {
    stack s_ob;
    queue q_ob;
    
    cout << "Демонстрація використання перевантажених операторів + та -\n\n";
    
    cout << "Додавання елементів в стек за допомогою оператора +\n";
    s_ob + 10 + 20 + 30;
    s_ob.display();
    
    cout << "\nДодавання елементів в чергу за допомогою оператора +\n";
    q_ob + 100 + 200 + 300;
    q_ob.display();
    
    cout << "\nВибірка елементів зі стеку за допомогою оператора -\n";
    cout << "Вибрано зі стеку: " << -s_ob << endl;
    s_ob.display();
    
    cout << "\nВибірка елементів з черги за допомогою оператора -\n";
    cout << "Вибрано з черги: " << -q_ob << endl;
    q_ob.display();
    
    cout << "\nЛанцюжкове використання оператора +\n";
    s_ob + 40 + 50 + 60;
    s_ob.display();
    
    q_ob + 400 + 500 + 600;
    q_ob.display();
    
    cout << "\nДемонстрація різниці між стеком (LIFO) та чергою (FIFO):\n";
    
    cout << "Вибірка зі стеку: " << -s_ob << ", " << -s_ob << ", " << -s_ob << endl;
    s_ob.display();
    
    cout << "Вибірка з черги: " << -q_ob << ", " << -q_ob << ", " << -q_ob << endl;
    q_ob.display();
    
    return 0;
}