#include <iostream>

// Тест на рекурсивну вбудовану функцію
inline int factorial(int n)
{
  if (n <= 1)
    return 1;
  return n * factorial(n - 1);
}

// Тест на вбудовану функцію зі статичною змінною
inline int counter()
{
  static int count = 0;
  return ++count;
}

// Тест на вбудовану функцію з циклом
inline int sum(int n)
{
  int total = 0;
  for (int i = 1; i <= n; i++)
  {
    total += i;
  }
  return total;
}

// Тест на вбудовану функцію з оператором switch
inline char grade(int score)
{
  switch (score / 10)
  {
  case 10:
  case 9:
    return 'A';
  case 8:
    return 'B';
  case 7:
    return 'C';
  case 6:
    return 'D';
  default:
    return 'F';
  }
}

int main()
{
  std::cout << "Тестування вбудованих функцій для вашого компілятора:" << std::endl;

  // Тест на рекурсивну вбудовану функцію
  std::cout << "Факторіал 5 (рекурсивна функція): " << factorial(5) << std::endl;

  // Тест на вбудовану функцію зі статичною змінною
  std::cout << "Лічильник викликів (статична змінна): "
            << counter() << ", " << counter() << ", " << counter() << std::endl;

  // Тест на вбудовану функцію з циклом
  std::cout << "Сума чисел від 1 до 10 (функція з циклом): " << sum(10) << std::endl;

  // Тест на вбудовану функцію з оператором switch
  std::cout << "Оцінка для 85 балів (функція з switch): " << grade(85) << std::endl;

  std::cout << std::endl;
  std::cout << "Примітка: Якщо всі функції спрацювали коректно, то ваш компілятор:" << std::endl;
  std::cout << "- Може підтримувати рекурсивні вбудовані функції" << std::endl;
  std::cout << "- Може підтримувати вбудовані функції зі статичними змінними" << std::endl;
  std::cout << "- Може підтримувати вбудовані функції з циклами" << std::endl;
  std::cout << "- Може підтримувати вбудовані функції з операторами switch" << std::endl;
  std::cout << std::endl;
  std::cout << "Однак, це не гарантує, що компілятор дійсно вбудовує ці функції." << std::endl;
  std::cout << "Для точного визначення можна використовувати відладчик або вивчити асемблерний код," << std::endl;
  std::cout << "згенерований компілятором з різними рівнями оптимізації." << std::endl;

  return 0;
}