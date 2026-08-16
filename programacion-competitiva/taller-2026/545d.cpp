#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  std::sort(a.begin(), a.end());

  long long sum = 0;
  int cnt = 0;

  for (int i = 0; i < n; i++) {
    if (sum > a[i])
      continue;
    cnt++;
    sum += a[i];
  }

  std::cout << cnt << std::endl;

  return 0;
}
