#include <bits/stdc++.h>

int main() {
  int n;
  std::cin >> n;

  std::vector<bool> criba(n + 1, 1);

  criba[0] = 0;
  criba[1] = 0;

  std::vector<long long> primes;

  for (long long i = 2; i * i <= n; i++) {
    if (criba[i]) {
      for (long long j = i * i; j <= n; j += i) {
        criba[j] = 0;
      }
    }
  }

  for (int i = 0; i < n + 1; i++) {
    if (criba[i]) {
      primes.push_back(i);
    }
  }

  int count = 0;

  for (long long i = 0; i < primes.size(); i++) {
    for (long long j = i + 1; j < primes.size(); j++) {
      std::cout << primes[i] << " " << primes[j] << " = "
                << primes[i] * primes[j] << std::endl;
      if (primes[i] * primes[j] <= n)
        count++;
    }
  }

  std::cout << count << std::endl;

  return 0;
}
