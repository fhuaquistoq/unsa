#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <tamano_matriz>\n";
    return 1;
  }

  const std::size_t matrix_size = std::atoi(argv[1]);

  std::vector<std::vector<double>> matrix_a(
      matrix_size, std::vector<double>(matrix_size));
  std::vector<double> vector_x(matrix_size);
  std::vector<double> vector_y(matrix_size, 0.0);

  for (std::size_t row = 0; row < matrix_size; ++row) {
    for (std::size_t column = 0; column < matrix_size; ++column) {
      matrix_a[row][column] =
          static_cast<double>(row * matrix_size + column + 1);
    }
  }
  for (std::size_t index = 0; index < matrix_size; ++index) {
    vector_x[index] = static_cast<double>(index + 1);
  }

  const std::chrono::steady_clock::time_point start_time =
      std::chrono::steady_clock::now();

  for (std::size_t column = 0; column < matrix_size; ++column) {
    for (std::size_t row = 0; row < matrix_size; ++row) {
      vector_y[row] += matrix_a[row][column] * vector_x[column];
    }
  }

  const std::chrono::steady_clock::time_point end_time =
      std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end_time - start_time;

  std::cout << "elapsed_seconds=" << std::fixed << std::setprecision(9)
            << elapsed.count() << '\n';

  return 0;
}
