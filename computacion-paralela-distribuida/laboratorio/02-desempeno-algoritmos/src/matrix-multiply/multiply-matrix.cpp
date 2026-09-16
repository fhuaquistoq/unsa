#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <tamano_matriz>\n";
    return 1;
  }

  const std::size_t matrix_size = std::stoull(argv[1]);

  std::vector<std::vector<double>> matrix_a(matrix_size,
                                            std::vector<double>(matrix_size));
  std::vector<std::vector<double>> matrix_b(matrix_size,
                                            std::vector<double>(matrix_size));
  std::vector<std::vector<double>> matrix_c(
      matrix_size, std::vector<double>(matrix_size, 0.0));

  for (std::size_t row = 0; row < matrix_size; ++row) {
    for (std::size_t column = 0; column < matrix_size; ++column) {
      const double value = static_cast<double>(row * matrix_size + column + 1);
      matrix_a[row][column] = value;
      matrix_b[row][column] = value;
    }
  }

  const std::chrono::steady_clock::time_point start_time =
      std::chrono::steady_clock::now();

  for (std::size_t row = 0; row < matrix_size; ++row) {
    for (std::size_t column = 0; column < matrix_size; ++column) {
      for (std::size_t inner = 0; inner < matrix_size; ++inner) {
        matrix_c[row][column] += matrix_a[row][inner] * matrix_b[inner][column];
      }
    }
  }

  const std::chrono::steady_clock::time_point end_time =
      std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end_time - start_time;

  std::cout << "elapsed_seconds=" << std::fixed << std::setprecision(9)
            << elapsed.count() << '\n';

  return 0;
}
