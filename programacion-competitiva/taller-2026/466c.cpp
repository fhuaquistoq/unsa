#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<long long> a(n + 1, 0);

  for (int i = 1; i <= n; i++) {
    std::cin >> a[i];
    a[i] += a[i - 1];
  }

  long long total = a[n];
  if (total % 3 != 0) {
    std::cout << 0 << std::endl;
    return 0;
  }

  long long sumValue = total / 3;

  std::vector<long long> suffix(n + 2, 0);
  for (int k = n - 1; k >= 1; k--) {
    suffix[k] = suffix[k + 1] + (a[k] == sumValue * 2 ? 1 : 0);
  }

  long long count = 0;
  for (int i = 1; i <= n; i++) {
    if (a[i] == sumValue) {
      count += suffix[i + 1];
    }
  }

  std::cout << count << std::endl;

  return 0;
}
