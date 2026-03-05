#include "correlate.h"
#include <algorithm>
#include <cmath>
#include <omp.h>
#include <vector>

// ============================================================
// VERSION 1 – Sequential baseline
// ============================================================
#ifdef SEQUENTIAL

void correlate(int ny, int nx, const float *data, float *result) {
  // Work with double precision
  std::vector<double> norm;

  // Normalize each row (mean=0, unit length) sequentially
  norm.resize((size_t)ny * nx);
  for (int y = 0; y < ny; y++) {
    double mean = 0.0;
    for (int x = 0; x < nx; x++)
      mean += data[x + y * nx];
    mean /= nx;

    double sq_sum = 0.0;
    for (int x = 0; x < nx; x++) {
      double v = data[x + y * nx] - mean;
      sq_sum += v * v;
    }
    double inv_std = (sq_sum > 0.0) ? 1.0 / std::sqrt(sq_sum) : 0.0;

    for (int x = 0; x < nx; x++)
      norm[x + y * nx] = (data[x + y * nx] - mean) * inv_std;
  }

  // Compute pairwise dot products (= Pearson correlation after normalization)
  for (int i = 0; i < ny; i++) {
    for (int j = 0; j <= i; j++) {
      double dot = 0.0;
      for (int x = 0; x < nx; x++)
        dot += norm[x + i * nx] * norm[x + j * nx];
      result[i + j * ny] = (float)dot;
    }
  }
}

// ============================================================
// VERSION 2 – OpenMP parallel
// ============================================================
#elif defined(PARALLEL)

// Normalize rows using OpenMP
static void normalize_rows(int ny, int nx, const float *data,
                           std::vector<double> &norm) {
  norm.resize((size_t)ny * nx);
#pragma omp parallel for schedule(dynamic, 4)
  for (int y = 0; y < ny; y++) {
    double mean = 0.0;
    for (int x = 0; x < nx; x++)
      mean += data[x + y * nx];
    mean /= nx;
    double sq_sum = 0.0;
    for (int x = 0; x < nx; x++) {
      double v = data[x + y * nx] - mean;
      sq_sum += v * v;
    }
    double inv_std = (sq_sum > 0.0) ? 1.0 / std::sqrt(sq_sum) : 0.0;
    for (int x = 0; x < nx; x++)
      norm[x + y * nx] = (data[x + y * nx] - mean) * inv_std;
  }
}

void correlate(int ny, int nx, const float *data, float *result) {
  std::vector<double> norm;
  normalize_rows(ny, nx, data, norm);

// Parallelize outer loop over row pairs
#pragma omp parallel for schedule(dynamic, 4)
  for (int i = 0; i < ny; i++) {
    for (int j = 0; j <= i; j++) {
      double dot = 0.0;
      for (int x = 0; x < nx; x++)
        dot += norm[x + i * nx] * norm[x + j * nx];
      result[i + j * ny] = (float)dot;
    }
  }
}

// ============================================================
// VERSION 3 – Optimized (OpenMP + cache-friendly tiling + SIMD-friendly loops)
// ============================================================
#else // OPTIMIZED (default)

// Tile size for cache-blocking
#define TILE 64

// Normalize rows using OpenMP
static void normalize_rows(int ny, int nx, const float *data,
                           std::vector<double> &norm) {
  norm.resize((size_t)ny * nx);
#pragma omp parallel for schedule(dynamic, 4)
  for (int y = 0; y < ny; y++) {
    double mean = 0.0;
    for (int x = 0; x < nx; x++)
      mean += data[x + y * nx];
    mean /= nx;
    double sq_sum = 0.0;
    for (int x = 0; x < nx; x++) {
      double v = data[x + y * nx] - mean;
      sq_sum += v * v;
    }
    double inv_std = (sq_sum > 0.0) ? 1.0 / std::sqrt(sq_sum) : 0.0;
    for (int x = 0; x < nx; x++)
      norm[x + y * nx] = (data[x + y * nx] - mean) * inv_std;
  }
}

void correlate(int ny, int nx, const float *data, float *result) {
  std::vector<double> norm;
  normalize_rows(ny, nx, data, norm);

// Zero-initialize result matrix
#pragma omp parallel for
  for (int i = 0; i < ny * ny; i++)
    result[i] = 0.0f;

// Cache-blocked matrix multiplication: result = norm * norm^T
// Only compute lower triangle (j <= i)
#pragma omp parallel for schedule(dynamic, 2) collapse(2)
  for (int i0 = 0; i0 < ny; i0 += TILE) {
    for (int j0 = 0; j0 < ny; j0 += TILE) {
      // Only process lower triangle tiles
      if (j0 > i0)
        continue;

      int i_end = std::min(i0 + TILE, ny);
      int j_end = std::min(j0 + TILE, ny);

      for (int i = i0; i < i_end; i++) {
        int jlim = std::min(i + 1, j_end); // enforce j <= i
        for (int j = j0; j < jlim; j++) {
          double dot = 0.0;
          // Inner loop – auto-vectorizable with -O2
          const double *ri = &norm[i * nx];
          const double *rj = &norm[j * nx];
#pragma omp simd reduction(+ : dot)
          for (int x = 0; x < nx; x++)
            dot += ri[x] * rj[x];
          result[i + j * ny] = (float)dot;
        }
      }
    }
  }
}

#endif
