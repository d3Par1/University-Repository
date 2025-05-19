#include <stdio.h>
#include <stdlib.h>

// Структура вузла списку
struct Node {
    int data;
    struct Node* next;
};

// Функція для створення нового вузла
struct Node* createNode(int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Помилка виділення пам'яті\n");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Додавання вузла в кінець списку
void append(struct Node** head, int data) {
    struct Node* newNode = createNode(data);
    
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    
    struct Node* last = *head;
    while (last->next != NULL) {
        last = last->next;
    }
    
    last->next = newNode;
}

// Виведення списку
void printList(struct Node* node) {
    printf("Елементи списку: ");
    while (node != NULL) {
        printf("%d ", node->data);
        node = node->next;
    }
    printf("\n");
}

// Створення циклічного списку
void makeCircular(struct Node** head) {
    if (*head == NULL) return;
    
    struct Node* last = *head;
    while (last->next != NULL) {
        last = last->next;
    }
    
    last->next = *head; // Зв'язуємо останній елемент з першим
}

// Перевірка, чи список циклічний
int isCircular(struct Node* head) {
    if (head == NULL) return 0;
    
    struct Node* slow = head;
    struct Node* fast = head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        
        if (slow == fast) return 1; // Знайдено цикл
    }
    
    return 0; // Цикл не знайдено
}

int main() {
    struct Node* head = NULL;
    
    // Додаємо елементи
    append(&head, 1);
    append(&head, 2);
    append(&head, 3);
    append(&head, 4);
    
    printList(head);
    
    // Перевіряємо, чи список циклічний
    printf("Список циклічний? %s\n", isCircular(head) ? "Так" : "Ні");
    
    // Робимо список циклічним
    makeCircular(&head);
    
    // Перевіряємо знову
    printf("Після перетворення, список циклічний? %s\n", isCircular(head) ? "Так" : "Ні");
    
    // Обережно виводимо циклічний список (обмежену кількість елементів)
    printf("Перші елементи циклічного списку: ");
    struct Node* temp = head;
    int count = 0;
    do {
        printf("%d ", temp->data);
        temp = temp->next;
        count++;
    } while (temp != head && count < 8); // Виводимо не більше 8 елементів
    printf("...\n");
    
    return 0;
}