#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <cantidad_de_datos> \n";
    return 1;
  }

  size_t MAX = std::atoi(argv[1]);

  std::vector<std::vector<double>> A(MAX, std::vector<double>(MAX, 1)),
      B(MAX, std::vector<double>(MAX, 1)), C(MAX, std::vector<double>(MAX, 0));

  const auto start_time = std::chrono::steady_clock::now();

  for (int i = 0; i < MAX; i++) {
    for (int j = 0; j < MAX; j++) {
      for (int k = 0; k < MAX; k++) {
        C[i][j] += (A[i][k] * B[k][j]);
      }
    }
  }

  const auto end_time = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end_time - start_time;

  std::cout << "elapsed_seconds=" << std::fixed << std::setprecision(9)
            << elapsed.count() << '\n';

  return 0;
}
