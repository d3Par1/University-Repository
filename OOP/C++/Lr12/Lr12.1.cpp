#include <iostream>
#include <string>
using namespace std;

template <typename T>
T myMin(T a, T b)
{
  return (a < b) ? a : b;
}

int main()
{
  cout << "min(3, 4) = " << myMin(3, 4) << endl;

  cout << "min('c', 'a') = " << myMin('c', 'a') << endl;

  cout << "min(3.14, 2.71) = " << myMin(3.14, 2.71) << endl;

  string str1 = "apple";
  string str2 = "banana";
  cout << "min(\"apple\", \"banana\") = " << myMin(str1, str2) << endl;

  return 0;
}