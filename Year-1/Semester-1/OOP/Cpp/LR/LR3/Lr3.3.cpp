#include <iostream>

// Об'єднання для обміну байтами
union ByteSwap
{
  int value;
  unsigned char bytes[sizeof(int)]; // масив байтів для доступу до окремих байтів
};

int main()
{
  ByteSwap num;

  // Запитуємо у користувача ціле число
  std::cout << "Введіть ціле число: ";
  std::cin >> num.value;

  std::cout << "Початкове число: " << num.value << std::endl;

  // Виведення побайтового представлення числа
  std::cout << "Побайтове представлення: ";
  for (size_t i = 0; i < sizeof(int); i++)
  {
    std::cout << static_cast<int>(num.bytes[i]) << " ";
  }
  std::cout << std::endl;

  // Обмін першого (молодшого) та останнього (старшого) байтів
  unsigned char temp = num.bytes[0];
  num.bytes[0] = num.bytes[sizeof(int) - 1];
  num.bytes[sizeof(int) - 1] = temp;

  std::cout << "Число після обміну байтів: " << num.value << std::endl;

  // Виведення побайтового представлення після обміну
  std::cout << "Побайтове представлення після обміну: ";
  for (size_t i = 0; i < sizeof(int); i++)
  {
    std::cout << static_cast<int>(num.bytes[i]) << " ";
  }
  std::cout << std::endl;

  return 0;
}