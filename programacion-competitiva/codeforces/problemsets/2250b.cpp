#include <bits/stdc++.h>

int main() {
  int t;
  std::cin >> t;

  while (t--) {
    int n, k;
    std::cin >> n >> k;

    if (n - k < 2) {
      std::cout << "-1\n";
    } else {
      std::string s = "";
      s.push_back('1');
      for (int i = 0; i < (k + 1) / 2; i++) {
        s.push_back('1');
      }
      s.push_back('0');
      for (int i = 0; i < k / 2; i++) {
        s.push_back('0');
      }

      char c = s.back() == '1' ? '0' : '1';
      int size = s.size();

      for (int i = 0; i < n - size; i++) {
        s.push_back(c);
        c = c == '1' ? '0' : '1';
      }

      std::cout << s << '\n';
    }
  }

  return 0;
}
