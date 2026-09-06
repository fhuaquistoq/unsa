#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Uso: " << argv[0]
              << " <cantidad_de_datos> <cantidad_de_threads>\n";
    return 1;
  }

  const std::size_t data_size = std::stoull(argv[1]);
  const int thread_count = std::stoi(argv[2]);
  if (thread_count <= 0) {
    std::cerr << "La cantidad de threads debe ser mayor que cero.\n";
    return 1;
  }

  omp_set_num_threads(thread_count);
  const auto start_time = std::chrono::steady_clock::now();
  std::vector<double> values(data_size, 1.0);
  double total_sum = 0.0;

#pragma omp parallel for reduction(+ : total_sum)
  for (std::size_t index = 0; index < data_size; ++index) {
    total_sum += values[index];
  }

  const auto end_time = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end_time - start_time;

  std::cout << std::setprecision(17) << "result=" << total_sum
            << " elapsed_seconds=" << std::fixed << std::setprecision(9)
            << elapsed.count() << '\n';
  return 0;
}
