#include "listrank.h"
#include "generators.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string.h>

#include "get_time.h"


int main(int argc, char* argv[]) {
  size_t n = 1e7;
  int num_rounds = 3;
  if (argc >= 2) {
    n = atoll(argv[1]);
  }
  if (argc >= 3) {
    num_rounds = atoi(argv[2]);
  }
  int flag = 0;
  if (argc >= 4) {
    flag = atoi(argv[3]);
  }
  bool test = 0;
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
    std::cout << "n = " << n << " num_rounds = " << num_rounds << " flag = " << flag << std::endl;
  }

  // Create a permutation encoding a singly-linked list.
  size_t* P;
  if (flag == 0) {
    P = CyclicPermutation(n);
  } else if (flag == 1) {
    P = ShiftedTabulate(n, 1);
  } else {
    std::cerr << "Unknown flag: " << flag << std::endl;
    exit(-1);
  }

  // Create the input list. Break the input permutation at 0, i.e.,
  // the first node is 0, and the last node is whichever node maps to
  // 0 in the permutation.
  ListNode* L = (ListNode*)malloc(n * sizeof(ListNode));
  auto reset_list = [&] () {
    parallel_for(0, n, [&](size_t i) {
      size_t next_id = P[i];
      if (next_id != 0) {
        L[i].next = L + next_id;
      } else {
        L[i].next = nullptr;
      }
      L[i].rank = std::numeric_limits<size_t>::max();
    });
  };
  reset_list();

  double total_time = 0;
  for (int i = 0; i <= num_rounds; i++) {
    parlay::timer t;
    SerialListRanking(L);
    t.stop();

    if (i == 0) {
      if(test && n>=5){
        std::cout << L[0].rank << " " << L[2].rank << " " << L[4].rank << " " << L[n-1].rank << std::endl;
      }
      else{
        std::cout << "Warmup round running time: " << t.total_time() << std::endl;
      }
    } else {
      if (!test){
        std::cout << "Round " << i << " running time: " << t.total_time()
                  << std::endl;
      }
      total_time += t.total_time();
    }
    reset_list();
  }
  if (!test){
    std::cout << "Average running time: " << total_time / num_rounds << std::endl;
  }

  // free(L);
  return 0;
}
