#include <bits/stdc++.h>

int main() {
  int n, m;
  std::cin >> n >> m;

  std::vector<long long> a(n), b(m);

  for (long long &ai : a)
    std::cin >> ai;

  for (long long &bi : b)
    std::cin >> bi;

  for (int i = 1; i < n; i++)
    a[i] += a[i - 1];

  for (long long bi : b) {
    long long room = std::lower_bound(a.begin(), a.end(), bi) - a.begin();
    long long local = bi - (room ? a[room - 1] : 0);
    std::cout << room + 1 << " " << local << '\n';
  }

  return 0;
}
