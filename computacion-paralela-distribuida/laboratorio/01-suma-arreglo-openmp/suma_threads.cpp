#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

double sum_range(const std::vector<double> &values, std::size_t begin,
                 std::size_t end) {
  double partial_sum = 0.0;
  for (std::size_t index = begin; index < end; ++index) {
    partial_sum += values[index];
  }
  return partial_sum;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Uso: " << argv[0]
              << " <cantidad_de_datos> <cantidad_de_threads>\n";
    return 1;
  }

  const std::size_t data_size = std::stoull(argv[1]);
  const std::size_t thread_count = std::stoull(argv[2]);
  if (thread_count == 0) {
    std::cerr << "La cantidad de threads debe ser mayor que cero.\n";
    return 1;
  }

  const auto start_time = std::chrono::steady_clock::now();
  std::vector<double> values(data_size, 1.0);
  std::vector<std::thread> workers;
  std::vector<double> partial_sums(thread_count, 0.0);
  workers.reserve(thread_count);

  const std::size_t base_chunk_size = data_size / thread_count;
  const std::size_t remainder = data_size % thread_count;
  std::size_t begin = 0;
  for (std::size_t thread_index = 0; thread_index < thread_count;
       ++thread_index) {
    const std::size_t chunk_size =
        base_chunk_size + (thread_index < remainder ? 1 : 0);
    const std::size_t end = begin + chunk_size;
    workers.emplace_back(
        [&, thread_index, begin, end]() {
          partial_sums[thread_index] = sum_range(values, begin, end);
        });
    begin = end;
  }

  for (std::thread &worker : workers) {
    worker.join();
  }

  double total_sum = 0.0;
  for (double partial_sum : partial_sums) {
    total_sum += partial_sum;
  }

  const auto end_time = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end_time - start_time;

  std::cout << std::setprecision(17) << "result=" << total_sum
            << " elapsed_seconds=" << std::fixed << std::setprecision(9)
            << elapsed.count() << '\n';
  return 0;
}
