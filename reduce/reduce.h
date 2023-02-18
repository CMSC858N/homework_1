#include "parallel.h"

using namespace parlay;

template <class T>
T serial_reduce(T *A, size_t n) {
  T tot = 0;
  for (size_t i=0; i<n; ++i) {
    tot += A[i];
  }
  return tot;
}

template <class T>
T reduce(T *A, size_t n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return A[0];
  } else {
    T v1, v2;
    auto f1 = [&]() { v1 = reduce(A, n / 2); };
    auto f2 = [&]() { v2 = reduce(A + n / 2, n - n / 2); };
    par_do(f1, f2);
    return v1 + v2;
  }
}
