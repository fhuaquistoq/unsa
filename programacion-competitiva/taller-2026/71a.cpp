#include <bits/stdc++.h>

int main() {
  int n;
  std::string s;

  std::cin >> n;

  while (n--) {
    std::cin >> s;

    if (s.size() <= 10) std::cout << s << "\n";
    else std::cout << s.front() << s.size() - 2 << s.back() << "\n";
  }

  return 0;
}
