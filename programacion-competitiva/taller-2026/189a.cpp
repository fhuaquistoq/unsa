#include <bits/stdc++.h>

int main() {
  int n, a, b, c;
  std::cin >> n >> a >> b >> c;

  std::vector<int> dp(n + 1, -1);
  dp[0] = 0;
  for (int i = 1; i <= n; i++) {
    for (int len : {a, b, c}) {
      if (i >= len && dp[i - len] != -1) {
        dp[i] = std::max(dp[i], dp[i - len] + 1);
      }
    }
  }

  std::cout << dp[n] << std::endl;

  return 0;
}
