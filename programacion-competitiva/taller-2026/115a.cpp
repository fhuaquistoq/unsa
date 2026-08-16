#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<std::vector<int>> v(n + 1);
  std::vector<int> roots;

  for (int i = 1; i <= n; i++) {
    int pi;
    std::cin >> pi;

    if (pi != -1) {
      v[i].push_back(pi);
      v[pi].push_back(i);
    } else {
      roots.push_back(i);
    }
  }

  return 0;
}
