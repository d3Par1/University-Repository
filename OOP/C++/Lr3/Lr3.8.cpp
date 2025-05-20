#include <iostream>

// Клас stack з прикладу 3.7 третьої лекції
class stack
{
  char stck[10]; // стек максимальної довжини 10
  int tos;       // позиція вершини стеку
public:
  // Конструктор
  stack() { tos = 0; }

  // Деструктор
  ~stack() {}

  // Розміщення елемента в стек
  void push(char ch)
  {
    if (tos == 10)
    {
      std::cout << "Стек заповнений!" << std::endl;
      return;
    }
    stck[tos] = ch;
    tos++;
  }

  // Вилучення елемента зі стеку
  char pop()
  {
    if (tos == 0)
    {
      std::cout << "Стек порожній!" << std::endl;
      return 0;
    }
    tos--;
    return stck[tos];
  }

  // Перевірка, чи стек порожній
  bool isEmpty()
  {
    return tos == 0;
  }

  // Перевірка, чи стек заповнений
  bool isFull()
  {
    return tos == 10;
  }

  // Отримання поточної позиції вершини стеку
  int getSize()
  {
    return tos;
  }

  // Отримання доступу до масиву стеку та його розміру
  // для дружньої функції showstack()
  friend void showstack(const stack &s);
};

// Функція для відображення вмісту стеку
void showstack(const stack &s)
{
  std::cout << "Вміст стеку:" << std::endl;

  // Якщо стек порожній
  if (s.tos == 0)
  {
    std::cout << "Стек порожній!" << std::endl;
    return;
  }

  // Виведення елементів стеку (від дна до вершини)
  std::cout << "Дно [ ";
  for (int i = 0; i < s.tos; i++)
  {
    std::cout << s.stck[i] << " ";
  }
  std::cout << "] Вершина" << std::endl;

  // Виведення елементів стеку (від вершини до дна)
  std::cout << "Вершина [ ";
  for (int i = s.tos - 1; i >= 0; i--)
  {
    std::cout << s.stck[i] << " ";
  }
  std::cout << "] Дно" << std::endl;
}

int main()
{
  stack myStack;

  // Додаємо кілька елементів у стек
  myStack.push('A');
  myStack.push('B');
  myStack.push('C');
  myStack.push('D');
  myStack.push('E');

  // Показуємо вміст стеку
  showstack(myStack);

  // Вилучаємо пару елементів
  std::cout << "\nВилучаємо два елементи:" << std::endl;
  std::cout << "Вилучено: " << myStack.pop() << std::endl;
  std::cout << "Вилучено: " << myStack.pop() << std::endl;

  // Знову показуємо вміст стеку
  std::cout << std::endl;
  showstack(myStack);

  // Додаємо ще елементи
  std::cout << "\nДодаємо ще елементи:" << std::endl;
  myStack.push('X');
  myStack.push('Y');
  myStack.push('Z');

  // Знову показуємо вміст стеку
  std::cout << std::endl;
  showstack(myStack);

  return 0;
}