#include <bits/stdc++.h>

int main() {
  int s, n;
  std::cin >> s >> n;

  std::vector<std::pair<int, int>> dragons(n);

  for (auto &dragon : dragons) {
    int x, y;
    std::cin >> x >> y;
    dragon.first = x;
    dragon.second = y;
  }

  std::sort(dragons.begin(), dragons.end());

  bool defeat = false;

  for (auto dragon : dragons) {
    if (s > dragon.first) {
      s += dragon.second;
    } else {
      defeat = true;
      break;
    }
  }

  std::cout << (defeat ? "NO" : "YES") << '\n';

  return 0;
}
