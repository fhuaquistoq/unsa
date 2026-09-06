#include <bits/stdc++.h>

int main() {
  int t;
  long long x, y;

  std::cin >> t;

  while (t--) {
    std::cin >> y >> x;

    long long num = 1;

    if (y > x) {
      if (y % 2 == 0) {
        num += (y / 2) * 3;
        num += (((y / 2) - 1) * (y / 2) / 2) * 8;
        num += ((y - 1) / 2);
        num -= (x - 1);
      } else {
        num += ((y - 1) / 2) * 3;
        num += ((((y - 1) / 2) - 1) * ((y - 1) / 2) / 2) * 8;
        num += (y / 2);
        num += (x - 1);
      }
    } else {
      if (x % 2 == 0) {
        num += ((x - 1) / 2) * 7;
        num += ((((x - 1) / 2) - 1) * ((x - 1) / 2) / 2) * 8;
        num += (x / 2);
        num += (y - 1);
      } else {
        num += (x / 2) * 7;
        num += (((x / 2) - 1) * (x / 2) / 2) * 8;
        num += ((x - 1) / 2);
        num -= (y - 1);
      }
    }

    std::cout << num << std::endl;
  }

  return 0;
}
