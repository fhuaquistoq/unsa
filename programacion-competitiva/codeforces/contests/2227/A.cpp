#include <iostream>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    int a, b;
    std::cin >> a >> b;

    if (a % 2 == 1 && b % 2 == 1)
      std::cout << "NO\n";
    else
      std::cout << "YES\n";
  }
}