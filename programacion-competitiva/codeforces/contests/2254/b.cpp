#include <bits/stdc++.h>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    int n;
    std::string s;
    std::cin >> n >> s;

    int index = -1;

    for (int i = 1; i < n - 1; i++)
    {
      if (s[i - 1] == s[i + 1] && s[i] != s[i - 1])
      {
        index = i;
        break;
      }
    }

    char c = s[0];
    int aux = 1;
    int count = 1;

    for (int i = 0; i < n; i++)
    {
      if (i == index)
        continue;

      if (s[i] == c)
      {
        aux++;
        continue;
      }
      else
      {
        c = s[i];
        if (aux == 1 && index == -1)
        {
          index = -2;
          continue;
        }
        aux = 1;
        count++;
      }
    }

    std::cout << count << std::endl;
  }

  return 0;
}