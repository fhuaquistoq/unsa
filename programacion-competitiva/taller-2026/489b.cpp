#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  int m;
  std::cin >> m;

  std::vector<int> b(m);
  for (int &bi : b)
    std::cin >> bi;

  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());

  int i = 0, j = 0, pairs = 0;

  while (i < n && j < m) {
    if (std::abs(a[i] - b[j]) <= 1) {
      pairs++;
      i++;
      j++;
    } else if (a[i] < b[j]) {
      i++;
    } else if (b[j] < a[i]) {
      j++;
    }
  }

  std::cout << pairs << '\n';

  return 0;
}
