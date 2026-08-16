#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<bool> criba(n + 1, 1);

  criba[0] = 0;
  criba[1] = 0;

  for (int i = 2; i * i <= n; i++) {
    if (criba[i]) {
      for (int j = i * i; j <= n; j += i) {
        criba[j] = 0;
      }
    }
  }

  int q;
  std::cin >> q;

  while (q--) {
    int val;
    std::cin >> val;

    std::cout << (criba[val] ? "Primo" : "No Primo") << std::endl;
  }

  return 0;
}
