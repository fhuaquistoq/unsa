#include <bits/stdc++.h>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    int n;
    std::cin >> n;

    bool isMaximiumPrime = (n + 1) % 2 || n == 1 ? true : false;

    for (int i = 3; i <= std::sqrt(n + 1); i += 2)
    {
      if ((n + 1) % i == 0)
      {
        isMaximiumPrime = false;
      }
    }

    std::cout << (isMaximiumPrime ? "YES" : "NO") << std::endl;
  }
  return 0;
}
