#include <bits/stdc++.h>

int main() {
  int t;
  std::cin >> t;

  while (t--) {
    int a, b, c;
    std::cin >> a >> b >> c;

    if (a == b || a == c || b == c) {
      std::cout << "0" << std::endl;
    } else {
      std::vector<int> values = {a, b, c};
      std::sort(values.begin(), values.end());
      std::cout << std::min(values[2] - values[1], values[1] - values[0])
                << '\n';
    }
  }

  return 0;
}
