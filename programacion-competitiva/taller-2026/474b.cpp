#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  for (int i = 1; i < n; i++) {
    a[i] += a[i - 1];
  }

  int m;
  std::cin >> m;

  while (m--) {
    int q;
    std::cin >> q;

    std::cout << std::lower_bound(a.begin(), a.end(), q) - a.begin() + 1
              << '\n';
  }

  return 0;
}
