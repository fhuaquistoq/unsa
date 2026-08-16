#include <bits/stdc++.h>

int main() {
  int n, count = 0, a, b, c, sum;
  std::cin >> n;

  while (n--) {
    std::cin >> a >> b >> c;

    sum = a + b + c;

    if (sum >= 2)
      count++;
  }

  std::cout << count << '\n';

  return 0;
}
