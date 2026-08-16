#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  std::sort(a.begin(), a.end());

  int q;
  std::cin >> q;

  while (q--) {
    int m;
    std::cin >> m;

    std::cout << std::upper_bound(a.begin(), a.end(), m) - a.begin() << '\n';
  }

  return 0;
}
