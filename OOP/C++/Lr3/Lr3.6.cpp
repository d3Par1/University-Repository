#include <iostream>

// Модифікований клас stack з вбудованими функціями
class stack
{
  char stck[10]; // стек максимальної довжини 10
  int tos;       // позиція вершини стеку
public:
  // Конструктор - вбудована функція
  inline stack() { tos = 0; }

  // Деструктор - вбудована функція
  inline ~stack() {}

  // Розміщення елемента в стек - вбудована функція
  inline void push(char ch)
  {
    if (tos == 10)
    {
      std::cout << "Стек заповнений!" << std::endl;
      return;
    }
    stck[tos] = ch;
    tos++;
  }

  // Вилучення елемента зі стеку - вбудована функція
  inline char pop()
  {
    if (tos == 0)
    {
      std::cout << "Стек порожній!" << std::endl;
      return 0;
    }
    tos--;
    return stck[tos];
  }

  // Перевірка, чи стек порожній - вбудована функція
  inline bool isEmpty()
  {
    return tos == 0;
  }

  // Перевірка, чи стек заповнений - вбудована функція
  inline bool isFull()
  {
    return tos == 10;
  }

  // Отримання поточної позиції вершини стеку - вбудована функція
  inline int getSize()
  {
    return tos;
  }
};

int main()
{
  stack s;

  // Додаємо елементи в стек
  std::cout << "Додаємо символи A-J в стек:" << std::endl;
  for (char ch = 'A'; ch <= 'J'; ch++)
  {
    std::cout << "Додаємо: " << ch << std::endl;
    s.push(ch);
  }

  // Пробуємо додати ще один елемент
  std::cout << "\nПробуємо додати ще один елемент:" << std::endl;
  s.push('K');

  // Вилучаємо елементи зі стеку
  std::cout << "\nВилучаємо елементи зі стеку:" << std::endl;
  while (!s.isEmpty())
  {
    std::cout << "Вилучаємо: " << s.pop() << std::endl;
  }

  // Пробуємо вилучити елемент з порожнього стеку
  std::cout << "\nПробуємо вилучити елемент з порожнього стеку:" << std::endl;
  s.pop();

  return 0;
}