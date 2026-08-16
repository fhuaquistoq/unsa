#include <bits/stdc++.h>

int main() {
  long long x;
  std::cin >> x;

  long long count = 0;

  while (x > 0) {
    if (x % 2 == 0) {
      x /= 2;
    } else {
      x -= 1;
      count++;
    }
  }

  std::cout << count << std::endl;

  return 0;
}
