#include <bits/stdc++.h>

int main() {
  int t;
  std::cin >> t;

  while (t--) {
    int n;
    std::cin >> n;

    std::vector<int> b(n);
    for (int &bi : b)
      std::cin >> bi;

    std::sort(b.rbegin(), b.rend());

    int x = b[0];
    int y = b[1];
    bool valid = true;

    for (int i = 0; i < n - 2; i++) {
      if (b[i + 2] != b[i] % b[i + 1]) {
        valid = false;
      }
    }

    if (valid) {
      std::cout << x << " " << y << std::endl;
    } else {
      std::cout << "-1" << std::endl;
    }
  }

  return 0;
}
