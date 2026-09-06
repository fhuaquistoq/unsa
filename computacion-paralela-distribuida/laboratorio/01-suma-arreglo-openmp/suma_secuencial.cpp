#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <cantidad_de_datos>\n";
    return 1;
  }

  const std::size_t data_size = std::stoull(argv[1]);
  const auto start_time = std::chrono::steady_clock::now();

  std::vector<double> values(data_size, 1.0);
  double total_sum = 0.0;
  for (double value : values) {
    total_sum += value;
  }

  const auto end_time = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end_time - start_time;

  std::cout << std::setprecision(17) << "result=" << total_sum
            << " elapsed_seconds=" << std::fixed << std::setprecision(9)
            << elapsed.count() << '\n';
  return 0;
}
