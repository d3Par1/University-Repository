#include <iostream>
#include <string>

// Клас, що представляє ресурс з динамічною пам'яттю
class ResourceHandler
{
private:
  std::string *data;
  bool isValid;

public:
  // Конструктор
  ResourceHandler(const std::string &content)
  {
    std::cout << "Виділення пам'яті для ресурсу" << std::endl;
    data = new std::string(content);
    isValid = true;
  }

  // Конструктор копіювання
  ResourceHandler(const ResourceHandler &other)
  {
    std::cout << "Копіювання ресурсу" << std::endl;
    data = new std::string(*other.data);
    isValid = other.isValid;
  }

  // Деструктор
  ~ResourceHandler()
  {
    if (data)
    {
      std::cout << "Звільнення пам'яті ресурсу: " << *data << std::endl;
      delete data;
      data = nullptr;
    }
  }

  // Метод, що виконує операцію з ресурсом
  void performOperation()
  {
    if (!isValid)
    {
      std::cout << "Помилка: Ресурс недійсний!" << std::endl;
      return;
    }
    std::cout << "Виконання операції з ресурсом: " << *data << std::endl;
  }

  // Метод для отримання даних
  std::string getData() const
  {
    return *data;
  }

  // Метод для зміни дійсності ресурсу
  void invalidate()
  {
    isValid = false;
  }
};

// Функція, яка створює ресурс, інвалідує його і повертає
// Це приклад некоректного повернення об'єкта з функції
ResourceHandler createInvalidResource()
{
  ResourceHandler resource("Тимчасовий ресурс");

  // Виконуємо дії з ресурсом
  resource.performOperation();

  // Інвалідуємо ресурс перед поверненням
  resource.invalidate();

  std::cout << "Ресурс інвалідовано всередині функції" << std::endl;

  // Повертаємо інвалідований ресурс
  return resource;
}

int main()
{
  std::cout << "Приклад помилкового повернення об'єкта з функції:" << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  // Отримуємо ресурс з функції
  ResourceHandler receivedResource = createInvalidResource();

  std::cout << "\nПробуємо використати отриманий ресурс:" << std::endl;
  receivedResource.performOperation(); // Спроба використати інвалідований ресурс

  std::cout << "\nПроблема в тому, що прапорець isValid був скопійований, але користувач" << std::endl;
  std::cout << "функції не знає, що ресурс вже невалідний. Це може призвести до" << std::endl;
  std::cout << "неправильного використання ресурсу або неочікуваної поведінки програми." << std::endl;

  std::cout << "\nАналогічно помилці звільнення динамічної пам'яті, повернення" << std::endl;
  std::cout << "інвалідованого об'єкта може створити важко виявлювані помилки," << std::endl;
  std::cout << "особливо якщо стан об'єкта не є очевидним з його інтерфейсу." << std::endl;

  return 0;
}

/*
Пояснення:
У цьому прикладі ми створюємо клас ResourceHandler, який керує динамічним ресурсом.
Функція createInvalidResource() створює об'єкт, виконує з ним деякі операції,
а потім інвалідує його перед поверненням.

Проблема в тому, що код, що викликає, не знає про те, що об'єкт вже інвалідовано,
і може намагатися використовувати його, що призведе до неочікуваної поведінки.

Це аналогічно помилці звільнення динамічної пам'яті:
- При звільненні динамічної пам'яті проблема в тому, що ви повертаєте вказівник на звільнену пам'ять
- У даному прикладі проблема в тому, що ви повертаєте об'єкт, який перебуває в недійсному стані

Обидві проблеми можуть призвести до важко відстежуваних помилок у програмі.
*/