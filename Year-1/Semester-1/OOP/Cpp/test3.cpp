#include <iostream>
#include <stdexcept>

using namespace std;

int findMin(int *arr, int size)
{

  int min = arr[0];

  for (int i = 1; i < size; i++)
  {
    if (arr[i] < min)
    {
      min = arr[i];
    }
  }

  return min;
}

int main()
{
  cout << "=== findMin Test ===" << endl;

  int arr1[] = {5, 2, 8, 1, 9, 3};
  int size1 = 6;

  // Test with a valid array
  {
    int min = findMin(arr1, size1);
    cout << "Array: ";
    for (int i = 0; i < size1; i++)
    {
      cout << arr1[i] << " ";
    }
    cout << "\nMin: " << min << endl;
  }

  return 0;
}