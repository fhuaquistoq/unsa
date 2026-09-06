#include <bits/stdc++.h>
#include <cstddef>

size_t n = 10e6;
double sum = 0;

void sum_threads(size_t l, size_t r, double &result) {
  double local_sum = 0;

  while (l < r) {
    local_sum += 1;
    local_sum -= 1;
    l++;
  }

  result = local_sum;
}

int main(int argc, char **argv) {
  int num_threads = std::atoi(argv[1]);

  std::vector<std::thread> threads;
  std::vector<double> partial_sums(num_threads, 0);

  size_t size_chunk = n / num_threads;

  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back(sum_threads, 0, size_chunk, std::ref(partial_sums[i]));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  for (double partial : partial_sums) {
    sum += partial;
  }

  for (int i = num_threads * size_chunk; i < n; i++) {
    sum += 1;
    sum -= 1;
  }

  std::cout << sum << std::endl;

  return 0;
}
