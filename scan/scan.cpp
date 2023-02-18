#include "scan.h"

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <string.h>

#include "get_time.h"

using Type = long long;

int main(int argc, char* argv[]) {
  size_t n = 1e9;
  int num_rounds = 3;
  bool func = 0;
  if (argc >= 2) {
    n = atoll(argv[1]);
  }
  if (argc >= 3) {
    num_rounds = atoi(argv[2]);
  }
  if (argc >= 4){
    func = atoi(argv[3]);
  }
  int test = 0;
  if (argc >= 5){
    if (!strcmp(argv[4],"-t")){
      test = 1;
    }
    else{
      std::cerr << "Unknown flag: " << argv[4] << std::endl;
      exit(-1);
    }
  }
  if (!test){
    std::cout << "n = " << n << " num_rounds = " << num_rounds << " func = " << (func ? "sum" : "max") << std::endl;
  }
  Type* A = (Type*)malloc(n * sizeof(Type));
  auto reset_array = [&] () {
    parallel_for(0, n, [&](size_t i) { A[i] = pow(-1,i*func)*(i+1); });
  };
  reset_array();

  double total_time = 0;
  for (int i = 0; i <= num_rounds; i++) {
    parlay::timer t;

    // A left-identity.
    Type id = func?std::numeric_limits<Type>::min():0;
    // An associative function f : Type x Type -> Type
    auto f = [&] (Type l, Type r) { return std::max(l,r); };
    auto g = [&] (Type l, Type r) { return l + r; };
    Type ans;
    if (func){
      // ans = scan_inplace(A, n, f, id);
      ans = scan_inplace_serial(A, n, f, id);
    } else{
      // ans = scan_inplace(A, n, g, id);
      ans = scan_inplace_serial(A, n, g, id);
    }
    t.stop();

    reset_array();

    if (i == 0) {
      if (test){
        std::cout << ans << std::endl;
      } else{
        std::cout << "Scan total: " << ans << std::endl;
        std::cout << "Warmup round running time: " << t.total_time() << std::endl;
      }
    } else {
      if (!test){
        std::cout << "Round " << i << " running time: " << t.total_time()
                  << std::endl;
      }
      total_time += t.total_time();
    }
  }
  if (!test){
    std::cout << "Average running time: " << total_time / num_rounds << std::endl;
  }

  free(A);
  return 0;
}
