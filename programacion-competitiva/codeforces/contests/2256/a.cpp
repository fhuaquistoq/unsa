#include <bits/stdc++.h>

int main()
{
  int t;
  std::cin >> t;

  while (t--)
  {
    int a, b, c;
    std::cin >> a >> b >> c;

    std::vector<int> v = {a, b, c};

    std::sort(v.begin(), v.end());

    int ci = v[0] + v[1];

    std::cout << std::min(v[2] - v[0], ci - v[0]) << std::endl;
  }

  return 0;
}