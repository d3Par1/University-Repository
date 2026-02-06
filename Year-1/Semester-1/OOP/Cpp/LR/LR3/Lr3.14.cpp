#include <iostream>
#include <cstring>

// Клас strtype з прикладу 3.2a третьої лекції
class strtype
{
  char *p;
  int len;

public:
  // Конструктор
  strtype(const char *ptr)
  {
    len = strlen(ptr) + 1;
    p = new char[len];
    strcpy(p, ptr);
  }

  // Деструктор
  ~strtype()
  {
    delete[] p;
  }

  // Метод для відображення рядка
  void show()
  {
    std::cout << p << " - довжина: " << len - 1 << std::endl;
  }

  // Оголошуємо дружню функцію get_string()
  friend char *get_string(strtype *s);
};

// Дружня функція для отримання вказівника на рядок
char *get_string(strtype *s)
{
  return s->p;
}

int main()
{
  // Створюємо об'єкт класу strtype
  strtype str1("Це тестовий рядок!");

  // Виводимо рядок, використовуючи метод show()
  std::cout << "Початковий рядок: ";
  str1.show();

  // Отримуємо вказівник на рядок за допомогою дружньої функції
  char *ptr = get_string(&str1);

  // Виводимо рядок, використовуючи отриманий вказівник
  std::cout << "Рядок через вказівник: " << ptr << std::endl;

  // Змінюємо рядок через вказівник (замінюємо перший символ на 'Ц')
  ptr[0] = 'Ц';

  // Виводимо змінений рядок
  std::cout << "Змінений рядок: ";
  str1.show();

  // Створюємо ще один об'єкт
  strtype str2("Інший рядок для тестування!");

  // Виводимо рядок
  std::cout << "\nДругий рядок: ";
  str2.show();

  // Отримуємо вказівник на рядок за допомогою дружньої функції
  ptr = get_string(&str2);

  // Виводимо рядок, використовуючи отриманий вказівник
  std::cout << "Другий рядок через вказівник: " << ptr << std::endl;

  return 0;
}