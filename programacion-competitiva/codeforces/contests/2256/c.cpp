#include <bits/stdc++.h>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    long long n, k;
    std::string s, aux;
    std::cin >> n >> k >> s;

    aux = s;

    long long red = 0, blue = 0;

    for (int i = 0; i < n * 2; i++)
    {
      if (s[i] == '1')
      {
        if (i % 2 == 0)
        {
          red++;
        }
        else
        {
          blue++;
        }
      }
    }

    for (int i = 0; i < n * 2 - 1; i++)
    {
      if (s[i] == '1' && s[i + 1] == '0')
      {
        if (i % 2 == 0)
        {
          red--;
          blue++;
        }
        else
        {
          blue--;
          red++;
        }
      }
    }

    if (s[n * 2 - 1] == '1' && s[0] == '0')
    {
      if ((n * 2 - 1) % 2 == 0)
      {
        red--;
        blue++;
      }
      else
      {
        blue--;
        red++;
      }
    }

    std::cout << blue << " " << red << std::endl;
  }

  return 0;
}