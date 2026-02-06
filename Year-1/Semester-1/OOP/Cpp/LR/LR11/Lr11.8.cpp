#include <iostream>
#include <algorithm> 
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
};

class sorted : public list {
private:
    int data[100];  
    int count;     
    int capacity;  
    
public:
    sorted() {
        count = 0;
        capacity = 100;
        head = NULL;
    }
    
    void store(int i) override {
        if (count >= capacity) {
            cout << "List is full.\n";
            return;
        }
        
        data[count++] = i;
        
        sort();
    }
    
    int retrieve() override {
        if (count == 0) {
            cout << "List empty.\n";
            return 0;
        }
        
        int value = data[0];
        
        for (int i = 0; i < count - 1; i++) {
            data[i] = data[i + 1];
        }
        
        count--;
        return value;
    }
    
    void sort() {
        std::sort(data, data + count);
    }
    
    void display() {
        if (count == 0) {
            cout << "List empty.\n";
            return;
        }
        
        cout << "Sorted list content: ";
        for (int i = 0; i < count; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    list *p;
    queue q_ob;
    stack s_ob;
    char ch;
    
    sorted sorted_list;
    
    for (int i=0; i<10; i++) {
        cout << "Stack or Queue? (S/Q): ";
        cin >> ch;
        ch = tolower(ch);
        
        if (ch == 'q')
            p = &q_ob;
        else
            p = &s_ob;
            
        p->store(i);
    }
    
    cout << "Enter T to terminate\n";
    
    for(;;) {
        cout << "Stack or Queue? (S/Q): ";
        cin >> ch;
        ch = tolower(ch);
        
        if (ch == 't') break;
        
        if (ch == 'q')
            p = &q_ob;
        else
            p = &s_ob;
            
        cout << p->retrieve() << '\n';
    }
    
    cout << "\nДемонстрація відсортованого списку:\n";
    
    for (int i = 0; i < 10; i++) {
        int val = rand() % 100;  
        cout << "Додаємо " << val << " до відсортованого списку\n";
        sorted_list.store(val);
        sorted_list.display();
    }
    
    cout << "\nВибірка елементів з відсортованого списку:\n";
    for (int i = 0; i < 5; i++) {
        int val = sorted_list.retrieve();
        cout << "Вибрано: " << val << endl;
        sorted_list.display();
    }
    
    cout << "\nПояснення щодо попереджень при компіляції:\n";
    cout << "1. Відсутність віртуального деструктора у базовому класі list.\n";
    cout << "   Це призводить до неправильного видалення об'єктів через вказівник на базовий клас.\n";
    cout << "2. Відсутність модифікаторів override для перевизначених віртуальних функцій.\n";
    cout << "   Це ускладнює відстеження помилок при перевизначенні функцій.\n\n";
    
    cout << "Динамічний поліморфізм слід використовувати в наступних випадках:\n";
    cout << "1. Коли потрібно використовувати один інтерфейс для роботи з об'єктами різних класів.\n";
    cout << "2. При розробці систем, де необхідно замінювати компоненти без зміни коду клієнта.\n";
    cout << "3. Для реалізації патернів проектування, як-от Стратегія, Команда, Спостерігач тощо.\n";
    cout << "4. Коли поведінка об'єктів повинна змінюватися під час виконання програми.\n";
    cout << "5. При створенні розширюваних фреймворків та бібліотек.\n";
    
    return 0;
}