#include <bits/stdc++.h>

int main() {
  int t, n;
  std::cin >> t;

  while (t--) {
    std::cin >> n;
    std::vector<int> a(n);

    for (int &ai : a)
      std::cin >> ai;

    std::cout << std::gcd(a[0], a[n - 1]) << std::endl;
  }

  return 0;
}
