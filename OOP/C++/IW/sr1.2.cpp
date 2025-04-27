#include <iostream>
using namespace std;

int main() {
  LinkedList list1;
  LinkedList list2;
  
  // Заповнення першого списку з клавіатури
  int n, value;
  do {
      cout << "Введіть кількість елементів для першого списку (від 5 до 10): ";
      cin >> n;
  } while (n < 5 || n > 10);
  
  cout << "Введіть " << n << " елементів:" << endl;
  for (int i = 0; i < n; i++) {
      cout << "Елемент " << i+1 << ": ";
      cin >> value;
      list1.append(value);
  }
  
  // Виведення першого списку
  cout << "Перший ";
  list1.display();
  
  // Заповнення другого списку заданими значеннями
  int predefinedValues[] = {100, 200, 300, 400, 500};
  for (int i = 0; i < 5; i++) {
      list2.append(predefinedValues[i]);
  }
  
  // Виведення другого списку
  cout << "Другий ";
  list2.display();
  
  // Злиття списків
  list1.merge(list2);
  
  // Виведення об'єднаного списку
  cout << "Об'єднаний ";
  list1.display();
  
  return 0;
}

// Клас вузла списку
class Node {
public:
    int data;      // Дані вузла
    Node* next;    // Вказівник на наступний вузол
    
    // Конструктор
    Node(int value) {
        data = value;
        next = nullptr;
    }
};

// Клас однозв'язного списку
class LinkedList {
private:
    Node* head;    // Голова списку
    
public:
    // Конструктор
    LinkedList() {
        head = nullptr;
    }
    
    // Деструктор
    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    
    // Додавання елемента в кінець списку
    void append(int value) {
        Node* newNode = new Node(value);
        
        // Якщо список порожній
        if (head == nullptr) {
            head = newNode;
            return;
        }
        
        // Знаходимо останній вузол
        Node* last = head;
        while (last->next != nullptr) {
            last = last->next;
        }
        
        // Додаємо новий вузол в кінець
        last->next = newNode;
    }
    
    // Виведення списку
    void display() {
        Node* current = head;
        cout << "Список: ";
        while (current != nullptr) {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }
    
    // Підрахунок елементів
    int count() {
        int count = 0;
        Node* current = head;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count;
    }
    
    // Отримання голови списку
    Node* getHead() {
        return head;
    }
    
    // Встановлення голови списку
    void setHead(Node* newHead) {
        head = newHead;
    }
    
    // Злиття двох списків
    void merge(LinkedList& list2) {
        // Якщо перший список порожній
        if (head == nullptr) {
            head = list2.getHead();
            return;
        }
        
        // Знаходимо останній вузол першого списку
        Node* last = head;
        while (last->next != nullptr) {
            last = last->next;
        }
        
        // Приєднуємо другий список до кінця першого
        last->next = list2.getHead();
        
        // Встановлюємо голову другого списку на nullptr,
        // щоб уникнути видалення даних при знищенні list2
        list2.setHead(nullptr);
    }
};

