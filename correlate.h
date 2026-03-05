#ifndef CORRELATE_H
#define CORRELATE_H

// Computes pairwise Pearson correlation coefficients between rows of 'data'.
// data   : ny x nx matrix, row-major (element [y][x] = data[x + y*nx])
// result : ny x ny output matrix; result[i + j*ny] = corr(row_i, row_j)
//          for all 0 <= j <= i < ny
void correlate(int ny, int nx, const float *data, float *result);

#endif // CORRELATE_H
