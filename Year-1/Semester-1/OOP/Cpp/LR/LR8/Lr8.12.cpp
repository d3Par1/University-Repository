#include <iostream>
using namespace std;

class coord
{
public:
  int x, y;
  coord() : x(0), y(0) {}
  coord(int i, int j) : x(i), y(j) {}
  void get_xy(int &i, int &j)
  {
    i = x;
    j = y;
  }
  friend coord operator+(coord ob1, coord ob2);
  friend coord operator-(coord ob1, coord ob2);
};

coord operator+(coord ob1, coord ob2)
{
  cout << "Using coord operator +()\n";
  return coord(ob1.x + ob2.x, ob1.y + ob2.y);
}

coord operator-(coord ob1, coord ob2)
{
  cout << "Using coord operator -()\n";
  return coord(ob1.x - ob2.x, ob1.y - ob2.y);
}

class quad : public coord
{
protected:
  int quadrant;

public:
  quad() : coord(), quadrant(0) {}
  quad(int x, int y) : coord(x, y)
  {
    calculateQuadrant();
  }

  void calculateQuadrant()
  {
    if (x >= 0 && y >= 0)
      quadrant = 1;
    else if (x < 0 && y >= 0)
      quadrant = 2;
    else if (x < 0 && y < 0)
      quadrant = 3;
    else
      quadrant = 4;
  }

  void showq()
  {
    cout << "Point in Quadrant: " << quadrant << '\n';
  }

  // Оператор присвоєння для quad = coord
  quad &operator=(const coord &ob2)
  {
    cout << "Using quad operator =()\n";
    x = ob2.x;
    y = ob2.y;
    calculateQuadrant();
    return *this;
  }

  // Оператор присвоєння для quad = quad
  quad &operator=(const quad &ob2)
  {
    cout << "Using quad operator =()\n";
    x = ob2.x;
    y = ob2.y;
    quadrant = ob2.quadrant;
    return *this;
  }
};

int main()
{
  quad o1(10, 10), o2(15, 3), o3;
  int x, y;

  o3 = o1 + o2;
  o3.get_xy(x, y);
  o3.showq();
  cout << "(o1+o2) X: " << x << ", Y: " << y << '\n';

  o3 = o1 - o2;
  o3.get_xy(x, y);
  o3.showq();
  cout << "(o1-o2) X: " << x << ", Y: " << y << '\n';

  o3 = o1;
  o3.get_xy(x, y);
  o3.showq();
  cout << "(o3=o1) X: " << x << ", Y: " << y << '\n';

  return 0;
}