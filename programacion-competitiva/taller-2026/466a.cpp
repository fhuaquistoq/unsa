#include <bits/stdc++.h>

int main() {
  int n, m, a, b;
  std::cin >> n >> m >> a >> b;

  long long costoMinMViajes = std::min(b, m * a);

  long long costoMin =
      costoMinMViajes * (n / m) + std::min((n % m) * a * 1LL, costoMinMViajes);

  std::cout << costoMin << std::endl;

  return 0;
}
