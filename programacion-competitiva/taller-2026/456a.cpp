#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<std::pair<int, int>> laptops(n);

  for (auto &laptop : laptops) {
    int a, b;
    std::cin >> a >> b;
    laptop.first = a;
    laptop.second = b;
  }

  std::sort(laptops.begin(), laptops.end(),
            [](std::pair<int, int> a, std::pair<int, int> b) {
              return a.first < b.second &&
                     a.second - a.first < b.second - b.first;
            });

  bool happy = false;

  std::cout << laptops[0].first << " - " << laptops[0].second << std::endl;

  for (int i = 1; i < n; i++) {
    std::cout << laptops[i].first << " - " << laptops[i].second << std::endl;
    if (laptops[0].first < laptops[i].first &&
        laptops[0].second > laptops[i].second) {
      happy = true;
      break;
    }
  }

  std::cout << (happy ? "Happy Alex" : "Poor Alex") << '\n';

  return 0;
}
