#include <iostream>
using namespace std;

class array
{
  int nums[10];

public:
  array();
  void set(int n[10]);
  void show();
  array operator+(array obj2);
  array operator-(array obj2);
  array operator==(array obj2);
  int getFirst() const;
};

array::array()
{
  for (int i = 0; i < 10; i++)
    nums[i] = 0; // Ініціалізуємо масив нулями
}

int array::getFirst() const
{
  return nums[0];
}

void array::set(int *n)
{
  for (int i = 0; i < 10; i++)
    nums[i] = n[i];
}

void array::show()
{
  for (int i = 0; i < 10; i++)
    cout << nums[i] << ' ';
  cout << "\n";
}

// Перевантажений оператор додавання - поелементне складання
array array::operator+(array obj2)
{
  array temp;
  for (int i = 0; i < 10; i++)
  {
    temp.nums[i] = nums[i] + obj2.nums[i];
  }
  return temp;
}

// Перевантажений оператор віднімання - поелементне віднімання
array array::operator-(array obj2)
{
  array temp;
  for (int i = 0; i < 10; i++)
  {
    temp.nums[i] = nums[i] - obj2.nums[i];
  }
  return temp;
}

// Перевантажений оператор порівняння - повертає об'єкт з 1 або 0 в першому елементі
array array::operator==(array obj2)
{
  array temp;
  for (int i = 0; i < 10; i++)
  {
    if (nums[i] != obj2.nums[i])
    {
      temp.nums[0] = 0; // false
      return temp;
    }
  }
  temp.nums[0] = 1; // true
  return temp;
}

int main()
{
  array obj1, obj2, obj3;
  int i[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  obj1.set(i);
  obj2.set(i);

  obj3 = obj1 + obj2;
  cout << "obj1 + obj2: ";
  obj3.show();

  obj3 = obj1 - obj3;
  cout << "obj1 - obj3: ";
  obj3.show();

  array result = obj1 == obj2;
  if (result.getFirst() == 1)
    cout << "obj1 equals obj2\n";
  else
    cout << "obj1 does not equal obj2\n";

  result = obj1 == obj3;
  if (result.getFirst() == 1)
    cout << "obj1 equals obj3\n";
  else
    cout << "obj1 does not equal obj3\n";

  return 0;
}