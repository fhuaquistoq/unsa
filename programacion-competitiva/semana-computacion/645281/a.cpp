#include <bits/stdc++.h>

int main()
{
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m, k;
  std::cin >> n >> m >> k;

  std::vector<std::string> s(n);
  for (std::string &si : s)
    std::cin >> si;

  auto dist = [&](int a, int b) -> int
  {
    int d = 0;
    for (int p = 0; p < m; p++)
      if (s[a][p] != s[b][p])
        d++;
    return d;
  };

  auto distEqualsK = [&](int a, int b) -> bool
  {
    int d = 0;
    for (int p = 0; p < m; p++)
    {
      if (s[a][p] != s[b][p])
      {
        d++;
        if (d > k)
          return false;
      }
    }
    return d == k;
  };

  const int ANCHORS = std::min(n, 8);
  std::vector<int> anchors;
  for (int i = 0; i < n && (int)anchors.size() < ANCHORS; i++)
  {
    bool distinct = true;
    for (int a : anchors)
      if (s[a] == s[i])
      {
        distinct = false;
        break;
      }
    if (distinct)
      anchors.push_back(i);
  }

  std::vector<char> valid(n, 1);

  for (int a : anchors)
  {
    std::vector<char> eqK(n);
    bool aIsSpecial = true;
    for (int j = 0; j < n; j++)
    {
      if (j == a)
      {
        eqK[j] = 1;
        continue;
      }
      eqK[j] = distEqualsK(a, j);
      if (!eqK[j])
        aIsSpecial = false;
    }

    if (aIsSpecial)
    {
      std::cout << a + 1 << std::endl;
      return 0;
    }

    valid[a] = 0;
    for (int j = 0; j < n; j++)
      if (j != a && !eqK[j])
        valid[j] = 0;
  }

  for (int i = 0; i < n; i++)
  {
    if (!valid[i])
      continue;

    bool ok = true;
    for (int j = 0; j < n && ok; j++)
    {
      if (j == i)
        continue;
      if (!distEqualsK(i, j))
        ok = false;
    }

    if (ok)
    {
      std::cout << i + 1 << std::endl;
      return 0;
    }
  }

  return 0;
}