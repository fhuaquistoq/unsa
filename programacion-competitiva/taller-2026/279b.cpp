#include <bits/stdc++.h>

int main() {
  int n, t;
  std::cin >> n >> t;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  int l = 0, sum = 0, best = 0;

  for (int r = 0; r < n; r++) {
    sum += a[r];
    while (sum > t) {
      sum -= a[l];
      l++;
    }
    best = std::max(best, r - l + 1);
  }

  std::cout << best << '\n';

  return 0;
}
