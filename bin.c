/* bin.c
 * Bentleys binary search adaption w/ Cilk and
 * reading targets from a text file
 * (C) Copyright 2024 Travis Montoya
 */
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include "ctimer.h"

#define bool_yn(x) (x > -1 ? 'y' : 'n')

int
bin_search(int *a, int k) {
  int l = 0;
  int u = 8;
  int m;

  while(l <= u) {
    m = l + (u - l) / 2;
    if (a[m] < k) {
      l = m + 1;
    } else if (a[m] == k) {
      return m;
    } else {
      u = m - 1;
    }
  }
  return -1;
}

int
main(void) {
  int i[9] = {1,2,2,3,6,7,8,9,20};
  int n;
  char res;

  ctimer_t ct;
  ctimer_start(&ct);

  cilk_scope {
    while(scanf("%d", &n) != EOF) {
        res = cilk_spawn bool_yn(bin_search(i, n));
        printf("%02d in array = %c\n", n, res);
    }
  }
  cilk_sync;

  ctimer_stop(&ct);
  ctimer_measure(&ct);

  ctimer_print(ct, "bin_search");

  return 0;
}
