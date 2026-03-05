#include "correlate.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <omp.h>
#include <vector>

// -------------------------------------------------------
// Print usage instructions
// -------------------------------------------------------
static void usage(const char *prog) {
  std::cerr << "Usage: " << prog << " <ny> <nx> [num_threads]\n"
            << "  ny           : number of vectors (rows)\n"
            << "  nx           : length of each vector (columns)\n"
            << "  num_threads  : (optional) number of OpenMP threads\n";
}

// -------------------------------------------------------
// Generate a random float matrix [ny x nx]
// -------------------------------------------------------
static void fill_random(int ny, int nx, std::vector<float> &mat) {
  std::srand(42); // fixed seed for reproducibility
  for (int i = 0; i < ny * nx; i++)
    mat[i] = static_cast<float>(std::rand()) / RAND_MAX;
}

// -------------------------------------------------------
// Print a small portion of the result for verification
// -------------------------------------------------------
static void print_sample(int ny, const std::vector<float> &result,
                         int limit = 5) {
  int show = std::min(ny, limit);
  std::cout << "\nSample of result matrix (top-left " << show << "x" << show
            << "):\n";
  for (int i = 0; i < show; i++) {
    for (int j = 0; j <= i && j < show; j++) {
      std::cout << result[i + j * ny];
      if (j < i)
        std::cout << "\t";
    }
    std::cout << "\n";
  }
}

// -------------------------------------------------------
// Main
// -------------------------------------------------------
int main(int argc, char *argv[]) {
  if (argc < 3) {
    usage(argv[0]);
    return 1;
  }

  int ny = std::atoi(argv[1]);
  int nx = std::atoi(argv[2]);

  if (ny <= 0 || nx <= 0) {
    std::cerr << "Error: ny and nx must be positive integers.\n";
    return 1;
  }

  // Optional: set number of threads
  if (argc >= 4) {
    int threads = std::atoi(argv[3]);
    if (threads > 0) {
      omp_set_num_threads(threads);
      std::cout << "Using " << threads << " OpenMP thread(s).\n";
    }
  } else {
    std::cout << "Using default OpenMP threads: " << omp_get_max_threads()
              << "\n";
  }

  std::cout << "Matrix size: ny=" << ny << ", nx=" << nx << "\n";

  // Allocate and fill input matrix
  std::vector<float> data(ny * nx);
  fill_random(ny, nx, data);

  // Allocate output result matrix
  std::vector<float> result(ny * ny, 0.0f);

  // ---- Time the correlate() call ----
  auto t_start = std::chrono::high_resolution_clock::now();

  correlate(ny, nx, data.data(), result.data());

  auto t_end = std::chrono::high_resolution_clock::now();
  double elapsed = std::chrono::duration<double>(t_end - t_start).count();

  std::cout << "correlate() completed in " << elapsed << " seconds.\n";

  // Verify diagonal elements are ~1.0 (self-correlation)
  bool diag_ok = true;
  for (int i = 0; i < ny; i++) {
    float v = result[i + i * ny];
    if (v < 0.99f || v > 1.01f) {
      diag_ok = false;
      std::cerr << "Warning: result[" << i << "][" << i << "] = " << v
                << " (expected ~1.0)\n";
    }
  }
  if (diag_ok)
    std::cout << "Diagonal check passed (all self-correlations ≈ 1.0).\n";

  // Print a small sample
  print_sample(ny, result);

  return 0;
}
