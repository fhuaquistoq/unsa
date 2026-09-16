#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Uso: " << argv[0] << " <tamano_matriz> <tamano_bloque>\n";
    return 1;
  }

  const std::size_t matrix_size = std::stoull(argv[1]);
  const std::size_t block_size = std::stoull(argv[2]);
  if (block_size == 0) {
    std::cerr << "El tamano de bloque debe ser mayor que cero.\n";
    return 1;
  }

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

  for (std::size_t block_row = 0; block_row < matrix_size;
       block_row += block_size) {
    const std::size_t row_end =
        std::min(block_row + block_size, matrix_size);
    for (std::size_t block_column = 0; block_column < matrix_size;
         block_column += block_size) {
      const std::size_t column_end =
          std::min(block_column + block_size, matrix_size);
      for (std::size_t block_inner = 0; block_inner < matrix_size;
           block_inner += block_size) {
        const std::size_t inner_end =
            std::min(block_inner + block_size, matrix_size);
        for (std::size_t row = block_row; row < row_end; ++row) {
          for (std::size_t column = block_column; column < column_end;
               ++column) {
            for (std::size_t inner = block_inner; inner < inner_end; ++inner) {
              matrix_c[row][column] +=
                  matrix_a[row][inner] * matrix_b[inner][column];
            }
          }
        }
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
