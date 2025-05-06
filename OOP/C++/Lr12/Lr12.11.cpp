#include <iostream>
#include <exception>
using namespace std;

void myTerminate()
{
  cout << "Викликано функцію myTerminate()" << endl;
  cout << "Програма завершується через необроблений виняток" << endl;
  exit(1);
}

void throwException()
{
  throw runtime_error("Тестовий виняток");
}

void throwTerminate() noexcept
{
  throw "Рядок винятку";
}

int main()
{
  set_terminate(myTerminate);

  cout << "Тестування функції terminate():" << endl;
  try
  {
    cout << "Викидаємо виняток без обробки..." << endl;
    throwException();
  }
  catch (const runtime_error &e)
  {
    cout << "Виняток оброблено: " << e.what() << endl;
  }

  cout << "\nТестування terminate() через noexcept:" << endl;
  try
  {
    cout << "Викидаємо виняток у noexcept-функції..." << endl;
    throwTerminate();
  }
  catch (...)
  {
    cout << "Оброблено виняток невідомого типу" << endl;
  }

  cout << "Програма завершена нормально." << endl;
  return 0;
}
