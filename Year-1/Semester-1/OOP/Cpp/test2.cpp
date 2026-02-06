#include <iostream>

using namespace std;

class Complex
{
private:
  double r, i;

public:
  Complex(double real = 0, double img = 0) : r(real), i(img) {}

  Complex operator+(const Complex &other) const
  {
    return Complex(r + other.r, i + other.i);
  }

  bool operator==(const Complex &other) const
  {
    return (r == other.r) && (i == other.i);
  }

  friend ostream &operator<<(ostream &os, const Complex &c)
  {
    os << c.r;
    if (c.i >= 0)
      os << "+";
    os << c.i << "i";
    return os;
  }
};

int main()
{
  Complex c1(3, 4);
  Complex c2(1, 2);

  Complex c3 = c1 + c2;

  cout << "c1 = " << c1 << endl;
  cout << "c2 = " << c2 << endl;
  cout << "c3 = " << c3 << endl;

  return 0;
}