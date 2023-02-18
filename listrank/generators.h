#include "random.h"

size_t* ShiftedTabulate(size_t n, size_t shift) {
  size_t* P = (size_t*)malloc(n * sizeof(size_t));
  for (size_t i=0; i<n; ++i) {
    P[i] = (i + shift) % n;
  }
  return P;
}

void CheckCyclic(size_t* P, size_t n) {
  std::cout << P[0] << std::endl;
  auto cur = P[0];
  size_t steps = 1;
  while (cur != 0) {
    cur = P[cur];
    ++steps;
  }
  if (steps != n) {
    std::cerr << "Not a cyclic permutation" << std::endl;
    exit(-1);
  }
}

size_t* CyclicPermutation(size_t n) {
  size_t* P = ShiftedTabulate(n, 0);
  size_t* I = (size_t*)malloc(n * sizeof(size_t));
  auto r = parlay::random(0);
  I[0] = 0;
  parlay::parallel_for(1, n, [&] (size_t i) {
    I[i] = r.ith_rand(i) % i;
  });
  for (long i=n-1; i > 0; --i) {
    std::swap(P[i], P[I[i]]);
  }
  // CheckCyclic(P, n);
  return P;
}