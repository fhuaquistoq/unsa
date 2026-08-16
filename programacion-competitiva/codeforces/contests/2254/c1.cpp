#include <bits/stdc++.h>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    int n;
    std::string a, b;
    std::cin >> n >> a >> b;

    if (a == b)
    {
      std::cout << "YES" << std::endl;
    }
    else if (n < 3 || std::count(a.begin(), a.end(), '1') != std::count(b.begin(), b.end(), '1'))
    {
      std::cout << "NO" << std::endl;
    }
    else
    {
      std::vector<int> a1, b1;
      for (int i = 0; i < n; i++)
      {
        if (a[i] == '1')
          a1.push_back(i);
        if (b[i] == '1')
          b1.push_back(i);
      }

      int cantOddA = 0, cantOddB = 0;
      for (int i = 0; i < a1.size(); i++)
      {
        if (a1[i] % 2 != 0)
          cantOddA++;
        if (b1[i] % 2 != 0)
          cantOddB++;
      }

      std::cout << (cantOddA == cantOddB ? "YES" : "NO") << std::endl;
    }
  }

  return 0;
}
