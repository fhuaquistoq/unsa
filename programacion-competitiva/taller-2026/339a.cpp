#include <bits/stdc++.h>

int main() {
  std::string s, tmp;
  std::cin >> s;

  for (int i = 0; i < s.size(); i += 2) {
    tmp += s[i];
  }

  std::sort(tmp.begin(), tmp.end());

  s.clear();

  for (char c : tmp) {
    s += c;
    s += '+';
  }

  s.pop_back();

  std::cout << s << '\n';

  return 0;
}
