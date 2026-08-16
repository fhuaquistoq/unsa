#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  std::sort(a.rbegin(), a.rend());

  int sumA = 0, sumB = std::accumulate(a.begin(), a.end(), 0);

  int idx = 0;

  while (sumA <= sumB) {
    sumA += a[idx];
    sumB -= a[idx];
    idx++;
  }

  std::cout << idx << '\n';

  return 0;
}
