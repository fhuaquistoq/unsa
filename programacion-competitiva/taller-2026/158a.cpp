#include <bits/stdc++.h>

int main() {
  int n, k;
  std::cin >> n >> k;

  std::vector<int> a(n);

  for (int &ai : a)
    std::cin >> ai;

  int score = a[k - 1];
  int count = 0;

  for (int ai : a) {
    if (ai >= score && ai > 0)
      count++;
  }

  std::cout << count << '\n';

  return 0;
}
