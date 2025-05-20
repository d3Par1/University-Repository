#include <iostream>
#include <cmath>

// Базовий клас Area
class Area
{
public:
  double height;
  double width;

  // Конструктор з параметрами
  Area(double h, double w) : height(h), width(w) {}
};

// Похідний клас Rectangle
class Rectangle : public Area
{
public:
  // Конструктор з використанням конструктора базового класу
  Rectangle(double h, double w) : Area(h, w) {}

  // Функція для обчислення площі прямокутника
  double area()
  {
    return height * width;
  }
};

// Похідний клас Isosceles (рівнобедрений трикутник)
class Isosceles : public Area
{
public:
  // Конструктор з використанням конструктора базового класу
  // height - висота, width - довжина основи
  Isosceles(double h, double w) : Area(h, w) {}

  // Функція для обчислення площі рівнобедреного трикутника
  double area()
  {
    return 0.5 * width * height;
  }
};

// Похідний клас Cylinder (циліндр)
class Cylinder : public Area
{
public:
  // Конструктор з використанням конструктора базового класу
  // height - висота циліндра, width - діаметр основи (D)
  Cylinder(double h, double w) : Area(h, w) {}

  // Функція для обчислення площі поверхні циліндра
  // 2*pi*R*R + pi*D*height, де R = D/2
  double area()
  {
    const double PI = 3.14159265358979323846;
    double radius = width / 2.0;
    return 2.0 * PI * radius * radius + PI * width * height;
  }
};

int main()
{
  // Створюємо об'єкти різних класів
  Rectangle rect(5.0, 4.0);
  Isosceles triangle(6.0, 3.0);
  Cylinder cylinder(10.0, 4.0);

  // Обчислюємо і виводимо площі
  std::cout << "Площа прямокутника: " << rect.area() << std::endl;
  std::cout << "Площа рівнобедреного трикутника: " << triangle.area() << std::endl;
  std::cout << "Площа поверхні циліндра: " << cylinder.area() << std::endl;

  return 0;
}