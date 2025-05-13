#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <new>
#include <cmath>
#include <ctime>
using namespace std;

// Ієрархія класів з покращеним деструктором
template <class T>
class Num
{
public:
  T x;
  Num(T i) { x = i; }
  virtual ~Num() {} // Віртуальний деструктор для коректного видалення об'єктів
  virtual T get_val() { return x; }
};

template <class T>
class Square : public Num<T>
{
public:
  Square(T i) : Num<T>(i) {}
  T get_val() override { return this->x * this->x; }
};

template <class T>
class Sqr_root : public Num<T>
{
public:
  Sqr_root(T i) : Num<T>(i) {}
  T get_val() override
  {
    // Безпечне обчислення з перевіркою на від'ємні значення
    if (this->x < 0)
      return static_cast<T>(0);
    return static_cast<T>(sqrt(static_cast<double>(this->x)));
  }
};

// Оновлена оригінальна функція generator
template <class T>
Num<T> *original_generator()
{
  switch (rand() % 2)
  {
  case 0:
    return new Square<T>(rand() % 100);
  case 1:
    return new Sqr_root<T>(rand() % 100);
  default:
    return nullptr; // Використовуємо nullptr замість NULL
  }
}

// Варіант 1: Відстеження помилок за допомогою винятків
template <class T>
Num<T> *exception_generator()
{
  try
  {
    switch (rand() % 2)
    {
    case 0:
      return new Square<T>(rand() % 100);
    case 1:
      return new Sqr_root<T>(rand() % 100);
    default:
      return nullptr;
    }
  }
  catch (const std::bad_alloc &e)
  {
    // Обробка винятку bad_alloc, який виникає при невдалому виділенні пам'яті
    cout << "Memory allocation error: " << e.what() << endl;
    return nullptr;
  }
}

// Варіант 2: Використання nothrow версії оператора new
template <class T>
Num<T> *nothrow_generator()
{
  switch (rand() % 2)
  {
  case 0:
  {
    // Використання nothrow версії new, яка не викидає виняток, а повертає nullptr при невдачі
    Square<T> *p = new (nothrow) Square<T>(rand() % 100);
    if (!p)
    {
      cout << "Memory allocation failed for Square" << endl;
      return nullptr;
    }
    return p;
  }
  case 1:
  {
    // Використання nothrow версії new для другого типу об'єктів
    Sqr_root<T> *p = new (nothrow) Sqr_root<T>(rand() % 100);
    if (!p)
    {
      cout << "Memory allocation failed for Sqr_root" << endl;
      return nullptr;
    }
    return p;
  }
  default:
    return nullptr;
  }
}

// Функція для тестування обробників помилок виділення пам'яті
template <class T>
void test_generator(const string &name, Num<T> *(*generator_func)())
{
  cout << "\n=== Using " << name << " ===\n";
  for (int i = 0; i < 3; i++)
  {
    Num<T> *p = generator_func();
    if (p)
    {
      cout << "Generated object value: " << p->get_val() << endl;

      // Виведення типу створеного об'єкта
      if (typeid(*p) == typeid(Square<T>))
        cout << "Object type: Square\n";
      else if (typeid(*p) == typeid(Sqr_root<T>))
        cout << "Object type: Sqr_root\n";

      delete p;
    }
    else
    {
      cout << "Failed to generate object\n";
    }
  }
}

// Симуляція нестачі пам'яті - демонстраційна функція
void simulate_memory_shortage()
{
  cout << "\n=== Memory error handling demonstration ===\n";
  cout << "Simulating memory shortage (this is just a simulation)\n";

  cout << "In a real memory shortage scenario:\n\n";
  cout << "1. Original generator would crash with uncaught bad_alloc exception.\n";
  cout << "2. Exception_generator would catch bad_alloc and return nullptr.\n";
  cout << "3. Nothrow_generator would check for nullptr and return nullptr.\n\n";
}

int main()
{
  // Ініціалізація генератора випадкових чисел
  srand(static_cast<unsigned int>(time(nullptr)));

  // Тестування всіх трьох генераторів
  test_generator<double>("original_generator", original_generator<double>);
  test_generator<double>("exception_generator", exception_generator<double>);
  test_generator<double>("nothrow_generator", nothrow_generator<double>);

  // Демонстрація обробки помилок пам'яті
  simulate_memory_shortage();

  return 0;
}