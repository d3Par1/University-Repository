#include <iostream>
#include <cstdlib> // для rand() і srand()
#include <ctime>   // для time()

// Клас Dice (гральний кубик)
class Dice
{
private:
  int value; // значення на кубику (1-6)

public:
  // Конструктор
  Dice()
  {
    value = 1; // початкове значення
  }

  // Функція для виконання кидка кубика
  void roll()
  {
    // Генерація випадкового числа від 1 до 6
    value = rand() % 6 + 1;
    // Виведення результату кидка
    std::cout << "Випало: " << value << std::endl;
  }

  // Отримання поточного значення
  int getValue()
  {
    return value;
  }
};

int main()
{
  // Ініціалізація генератора випадкових чисел поточним часом
  srand(static_cast<unsigned int>(time(0)));

  // Створення чотирьох кубиків
  Dice dice1, dice2, dice3, dice4;

  std::cout << "Кидаємо 4 кубики по 5 разів кожен:" << std::endl;

  // Виконуємо 5 кидків чотирма кубиками
  for (int roll = 1; roll <= 5; roll++)
  {
    std::cout << "\nКидок #" << roll << ":" << std::endl;

    std::cout << "Кубик 1: ";
    dice1.roll();

    std::cout << "Кубик 2: ";
    dice2.roll();

    std::cout << "Кубик 3: ";
    dice3.roll();

    std::cout << "Кубик 4: ";
    dice4.roll();

    // Підраховуємо та виводимо суму всіх кубиків
    int sum = dice1.getValue() + dice2.getValue() + dice3.getValue() + dice4.getValue();
    std::cout << "Сума цього кидка: " << sum << std::endl;
  }

  return 0;
}