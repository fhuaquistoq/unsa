#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::string s;

  while (n--) {
    std::cin >> s;
    int f0 = -1, f1 = -1;

    for (int i = 0; i < s.size(); i++) {
      if (s[i] == '0')
        f0 = i;
      if (s[i] == '1')
        f1 = i;
      if (f0 >= 0 && f1 >= 0)
        break;
    }

    for (int i = 0; i < s.size(); i++) {
      if (f0 == i || f1 == i)
        continue;
      std::cout << s[i];
    }
    std::cout << '\n';
  }

  return 0;
}
