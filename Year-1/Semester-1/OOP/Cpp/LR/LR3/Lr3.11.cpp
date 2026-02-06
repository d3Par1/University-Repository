#include <iostream>

// Клас для роботи з першим принтером
// Forward declaration of pr2 for friend function declaration in pr1
class pr2;

class pr1
{
  int printing;
  //...
public:
  pr1() { printing = 0; }
  void set_print(int status) { printing = status; }
  // Оголошуємо дружню функцію inuse()
  friend bool inuse(const pr1 &p1, const pr2 &p2);
  // ...
};

// Клас для роботи з другим принтером
class pr2
{
  int printing;
  // ...
public:
  pr2() { printing = 0; }
  void set_print(int status) { printing = status; }
  // Оголошуємо дружню функцію inuse()
  friend bool inuse(const pr1 &p1, const pr2 &p2);
  // ...
};

// Функція для перевірки, чи використовується принтер
bool inuse(const pr1 &p1, const pr2 &p2)
{
  // Повертаємо true, якщо хоча б один із принтерів зайнятий
  return (p1.printing != 0 || p2.printing != 0);
}

int main()
{
  pr1 printer1;
  pr2 printer2;

  // Спочатку обидва принтери вільні
  std::cout << "Спочатку принтер використовується: "
            << (inuse(printer1, printer2) ? "Так" : "Ні") << std::endl;

  // Займаємо перший принтер
  printer1.set_print(1);
  std::cout << "Після зайняття printer1: "
            << (inuse(printer1, printer2) ? "Так" : "Ні") << std::endl;

  // Звільняємо перший принтер і займаємо другий
  printer1.set_print(0);
  printer2.set_print(1);
  std::cout << "Після звільнення printer1 і зайняття printer2: "
            << (inuse(printer1, printer2) ? "Так" : "Ні") << std::endl;

  // Звільняємо другий принтер
  printer2.set_print(0);
  std::cout << "Після звільнення printer2: "
            << (inuse(printer1, printer2) ? "Так" : "Ні") << std::endl;

  return 0;
}