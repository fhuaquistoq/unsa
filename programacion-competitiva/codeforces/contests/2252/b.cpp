#include <bits/stdc++.h>

int main()
{
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int t;
  std::cin >> t;

  while (t--)
  {
    int n;
    std::string s;
    std::cin >> n >> s;

    int count0 = std::count(s.begin(), s.end(), '0');
    int count1 = std::count(s.begin(), s.end(), '1');

    int D = count0 - count1;

    if (std::abs(D) > 2)
    {
      std::cout << "-1" << '\n';
      continue;
    }

    std::string r;
    r += s[0];

    for (int i = 0; i < n; i++)
      if (r.back() != s[i])
        r += s[i];

    auto calcDiference = [](char startChar, int len)
    {
      if (len == 0)
        return 0;
      if (len % 2 == 0)
        return 0;
      return (startChar == '0') ? 1 : -1;
    };

    int best = -1;

    {
      int len = r.size();
      char st = r[0];
      int k = calcDiference(st, len);
      if (std::abs(D - k) <= 1)
        best = std::max(best, len);
    }

    if (r.size() > 1)
    {
      int len = r.size() - 1;
      char st = r[0];
      int k = calcDiference(st, len);
      if (std::abs(D - k) <= 1)
        best = std::max(best, len);
    }

    if (r.size() > 1)
    {
      int len = r.size() - 1;
      char st = r[1];
      int k = calcDiference(st, len);
      if (std::abs(D - k) <= 1)
        best = std::max(best, len);
    }

    if (r.size() > 2)
    {
      int len = r.size() - 2;
      char st = r[1];
      int k = calcDiference(st, len);
      if (std::abs(D - k) <= 1)
        best = std::max(best, len);
    }

    std::cout << (n - best) << '\n';
  }
  return 0;
}