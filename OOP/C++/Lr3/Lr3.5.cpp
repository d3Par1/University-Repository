#include <iostream>

// Вбудовані функції для знаходження абсолютного значення
// різних типів даних

// Версія для int
inline int abs(int n)
{
  return (n < 0) ? -n : n;
}

// Версія для long
inline long abs(long n)
{
  return (n < 0) ? -n : n;
}

// Версія для double
inline double abs(double n)
{
  return (n < 0) ? -n : n;
}

int main()
{
  int i = -10;
  long l = -100000L;
  double d = -123.45;

  std::cout << "Абсолютне значення " << i << " = " << abs(i) << std::endl;
  std::cout << "Абсолютне значення " << l << " = " << abs(l) << std::endl;
  std::cout << "Абсолютне значення " << d << " = " << abs(d) << std::endl;

  // Тест з додатними значеннями
  i = 20;
  l = 200000L;
  d = 456.78;

  std::cout << "\nАбсолютне значення " << i << " = " << abs(i) << std::endl;
  std::cout << "Абсолютне значення " << l << " = " << abs(l) << std::endl;
  std::cout << "Абсолютне значення " << d << " = " << abs(d) << std::endl;

  return 0;
}