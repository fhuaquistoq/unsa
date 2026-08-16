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

    long long mod = 998244353;
    long long ans = 0;

    std::string starts[4] = {"00", "01", "10", "11"};

    for (int k = 0; k < 4; k++)
    {
      bool ok = true;
      for (int i = 0; i < n && ok; i++)
      {
        char c = "0011"[(i + k) % 4];
        if (s[i] != '?' && s[i] != c)
          ok = false;
      }
      if (ok)
        ans++;
    }

    std::cout << ans % mod << std::endl;
  }

  return 0;
}