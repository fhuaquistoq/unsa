#include <bits/stdc++.h>
#include <numeric>
#include <vector>

int main() {
  int t;
  std::cin >> t;

  while (t--) {
    int n;
    std::cin >> n;

    std::vector<int> a(n);
    for (int &ai : a)
      std::cin >> ai;

    long long sum = std::accumulate(a.begin(), a.end(), 0);
  }

  return 0;
}
