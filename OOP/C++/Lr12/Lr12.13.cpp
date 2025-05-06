#include <iostream>
#include <complex>
#include <cmath>
using namespace std;

template <typename T>
T abs(T val)
{
  return (val < 0) ? -val : val;
}

double abs(const complex<double> &val)
{
  return sqrt(val.real() * val.real() + val.imag() * val.imag());
}

int main()
{
  cout << "abs(-5) = " << abs(-5) << endl;
  cout << "abs(10) = " << abs(10) << endl;

  cout << "abs(-3.14) = " << abs(-3.14) << endl;
  cout << "abs(2.71) = " << abs(2.71) << endl;

  complex<double> c1(3, 4);
  cout << "abs(3 + 4i) = " << abs(c1) << " (має бути 5)" << endl;

  complex<double> c2(-5, 12);
  cout << "abs(-5 + 12i) = " << abs(c2) << " (має бути 13)" << endl;

  long long bigNum = -9223372036854775807LL;
  cout << "abs(" << bigNum << ") = " << abs(bigNum) << endl;

  return 0;
}