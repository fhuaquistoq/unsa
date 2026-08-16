#include <bits/stdc++.h>

int main() {
  int n, k;
  std::cin >> n >> k;

  std::vector<int> a(n);
  for (int &ai : a)
    std::cin >> ai;

  int minSum = 0;
  int minIdx = 1;

  for (int i = 0; i < k; i++) {
    minSum += a[i];
  }

  int currSum = minSum;

  for (int i = k; i < n; i++) {
    currSum -= a[i - k];
    currSum += a[i];
    if (currSum < minSum) {
      minSum = currSum;
      minIdx = i - k + 1 + 1;
    }
  }

  std::cout << minIdx << '\n';

  return 0;
}
