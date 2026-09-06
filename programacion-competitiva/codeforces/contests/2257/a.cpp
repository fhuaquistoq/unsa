#include <bits/stdc++.h>

int main() {
  int t;
  std::cin >> t;

  while (t--) {
    int n, m;
    std::cin >> n >> m;
    std::vector<bool> initials(30, 0);

    std::string si;

    for (int i = 0; i < n; i++) {
      std::cin >> si;
      initials[si[0] - 'a'] = 1;
    }

    bool flag = 1;

    for (int i = 0; i < m; i++) {
      std::cin >> si;

      for (int j = 0; j < si.size(); j++) {
        if (!initials[si[j] - 'A'])
          flag = 0;
      }
    }

    std::cout << (flag ? "YES" : "NO") << std::endl;
  }
}
