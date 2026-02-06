#include <iostream>

// Клас who
class who
{
private:
  char id; // ідентифікатор об'єкта

public:
  // Конструктор
  who(char identifier)
  {
    id = identifier;
    std::cout << "Constructing who #" << id << std::endl;
  }

  // Деструктор
  ~who()
  {
    std::cout << "Destroying who #" << id << std::endl;
  }

  // Отримання ідентифікатора
  char getId()
  {
    return id;
  }
};

// Функція, що створює та повертає об'єкт who
who make_who()
{
  who temp('Z');
  std::cout << "Об'єкт who #" << temp.getId() << " створено у функції make_who()" << std::endl;
  return temp; // Повертаємо об'єкт (буде створена копія)
}

int main()
{
  std::cout << "Початок виконання функції main()" << std::endl;

  // Створюємо перший об'єкт
  std::cout << "\nСтворення об'єкта obj1:" << std::endl;
  who obj1('A');

  {
    // Створюємо другий об'єкт у новому блоці видимості
    std::cout << "\nСтворення об'єкта obj2 у блоці:" << std::endl;
    who obj2('B');

    // Створюємо третій об'єкт
    std::cout << "\nСтворення об'єкта obj3:" << std::endl;
    who obj3('C');

    std::cout << "\nВихід з блоку" << std::endl;
  } // obj2 і obj3 знищуються тут

  // Викликаємо функцію make_who()
  std::cout << "\nВиклик функції make_who():" << std::endl;
  who obj4 = make_who();
  std::cout << "Об'єкт who #" << obj4.getId() << " отримано з функції make_who()" << std::endl;

  std::cout << "\nКінець виконання функції main()" << std::endl;
  return 0;
} // obj1 і obj4 знищуються тут

/* Пояснення результатів:
1. При створенні об'єктів виводиться повідомлення "Constructing who #X"
2. При знищенні об'єктів виводиться повідомлення "Destroying who #X"
3. Об'єкти, створені в блоці, знищуються при виході з блоку
4. Коли функція make_who() повертає об'єкт:
   - Створюється тимчасовий об'єкт 'Z' у функції
   - Його значення копіюється в об'єкт obj4
   - Тимчасовий об'єкт знищується після повернення з функції
   - Об'єкт obj4 знищується в кінці main()
5. Порядок знищення об'єктів зворотний до порядку їх створення
*/