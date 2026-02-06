#include <iostream>
#include <string>

// Визначаємо константи для булевих значень
#define yes true
#define no false

// Перелічення для кольорів фруктів
enum Color
{
  red,
  green,
  yellow,
  orange
};

// Базовий клас Fruit
class Fruit
{
protected:
  std::string name;
  Color color;
  bool sweet;
  bool seedless;
  bool edible;

public:
  // Конструктор
  Fruit()
  {
    name = "";
    color = red;
    sweet = false;
    seedless = false;
    edible = true;
  }

  // Метод для виведення інформації про фрукт
  void show()
  {
    std::cout << "Name: " << name << std::endl;
    std::cout << "Color: ";
    switch (color)
    {
    case red:
      std::cout << "red";
      break;
    case green:
      std::cout << "green";
      break;
    case yellow:
      std::cout << "yellow";
      break;
    case orange:
      std::cout << "orange";
      break;
    }
    std::cout << std::endl;
    std::cout << "Sweet: " << (sweet ? "yes" : "no") << std::endl;
    std::cout << "Seedless: " << (seedless ? "yes" : "no") << std::endl;
    std::cout << "Edible: " << (edible ? "yes" : "no") << std::endl;
  }
};

// Похідний клас Apple
class Apple : public Fruit
{
public:
  // Існуючий метод seta
  void seta(std::string n, Color c)
  {
    name = n;
    color = c;
  }

  // Перевантажений метод seta з додатковими параметрами
  void seta(std::string n, Color c, bool swt, bool sdls, bool edb)
  {
    name = n;
    color = c;
    sweet = swt;
    seedless = sdls;
    edible = edb;
  }
};

// Похідний клас Orange
class Orange : public Fruit
{
public:
  // Існуючий метод seto
  void seto(std::string n, Color c)
  {
    name = n;
    color = c;
  }

  // Перевантажений метод seto з додатковими параметрами
  void seto(std::string n, Color c, bool swt, bool sdls, bool edb)
  {
    name = n;
    color = c;
    sweet = swt;
    seedless = sdls;
    edible = edb;
  }
};

// Головна функція
int main()
{
  Apple a2;
  Orange o2;

  // Використовуємо перевантажені методи
  a2.seta("Jonathan", red, yes, no, yes);
  o2.seto("Valencia", orange, yes, yes, no);

  // Виводимо інформацію про фрукти
  std::cout << "Інформація про яблуко:" << std::endl;
  a2.show();

  std::cout << "\nІнформація про апельсин:" << std::endl;
  o2.show();

  return 0;
}