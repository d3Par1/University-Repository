#include <iostream>
using namespace std;
class Base;
class BaseFriend {
public:
  void printResult(Base& b);
};
class Base {
private:
  float a;
  float power = 0;
public:
  void setA(float l) {
    a = l;
    power = a * a;
  }
  friend void BaseFriend::printResult(Base&);
};
void BaseFriend::printResult(Base& b) {
  cout << b.power << endl;
}
int main()
{
  Base b;
  b.setA(44);
  BaseFriend bf;
  bf.printResult(b);
}