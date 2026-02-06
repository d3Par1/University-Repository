#include <iostream>

// Базовий клас
class base
{
  int a;

public:
  void load_a(int n) { a = n; }
  int get_a() { return a; }
};

// Похідний клас
class derived : public base
{
  int b;

public:
  void load_b(int n) { b = n; }
  int get_b() { return b; }
};

int main()
{
  std::cout << "Експеримент з присвоєнням об'єктів похідного класу:" << std::endl;
  std::cout << "--------------------------------------------------------" << std::endl;

  // Створюємо та ініціалізуємо перший об'єкт похідного класу
  derived obj1;
  obj1.load_a(10); // Ініціалізуємо змінну базового класу
  obj1.load_b(20); // Ініціалізуємо змінну похідного класу

  std::cout << "Перший об'єкт (obj1):" << std::endl;
  std::cout << "obj1.a = " << obj1.get_a() << std::endl;
  std::cout << "obj1.b = " << obj1.get_b() << std::endl;

  // Створюємо другий об'єкт похідного класу
  derived obj2;

  // Присвоюємо перший об'єкт другому
  obj2 = obj1;

  std::cout << "\nДругий об'єкт (obj2) після присвоєння obj2 = obj1:" << std::endl;
  std::cout << "obj2.a = " << obj2.get_a() << std::endl;
  std::cout << "obj2.b = " << obj2.get_b() << std::endl;

  // Змінюємо значення в першому об'єкті
  obj1.load_a(30);
  obj1.load_b(40);

  std::cout << "\nПерший об'єкт (obj1) після зміни:" << std::endl;
  std::cout << "obj1.a = " << obj1.get_a() << std::endl;
  std::cout << "obj1.b = " << obj1.get_b() << std::endl;

  std::cout << "\nДругий об'єкт (obj2) після зміни obj1:" << std::endl;
  std::cout << "obj2.a = " << obj2.get_a() << std::endl;
  std::cout << "obj2.b = " << obj2.get_b() << std::endl;

  std::cout << "\nВИСНОВОК:" << std::endl;
  std::cout << "При присвоєнні об'єкта похідного класу іншому об'єкту" << std::endl;
  std::cout << "того ж класу копіюються як дані похідного класу," << std::endl;
  std::cout << "так і дані базового класу. Це підтверджується тим, що" << std::endl;
  std::cout << "після присвоєння obj2 = obj1 значення obj2.a дорівнює obj1.a," << std::endl;
  std::cout << "а після зміни obj1 значення obj2 залишаються незмінними." << std::endl;

  return 0;
}