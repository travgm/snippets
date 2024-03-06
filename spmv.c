/*spmv.c
 * Sparse vector multiplication, The spmv() function is taken
 * from the performance engineering lectures at MIT. This just
 * shows an example of it's use, although in sparse vector
 * multiplication the vector usually consists of mostly 0's, but
 * this example should suffice.
 * (C) Copyright 2024 Travis Montoya
 */
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include "ctimer.h"

typedef struct {
  int n, nnz;
  int *rows;
  int *cols;
  double *vals;
} sparse_matrix_t;

void 
spmv(sparse_matrix_t *A, double *x, double *y) {
  cilk_scope {
    cilk_spawn for (int i = 0; i < A->n; i++) {
      y[i] = 0;
      cilk_spawn for (int k = A->rows[i]; k < A->rows[i+1]; k++) {
        int j = A->cols[k];
        y[i] += A->vals[k] * x[j];
      }
    }
  }
  cilk_sync;
}

/* Begin test for example */
int
main(void) {
  double out[4];
  double inp[8] = {6.3, 2.5, 
                   8.5, 3.3, 
                   1.0, 9.5, 
                   3.1, 9.0};
  double val[8] = {1.0, 3.0, 
                   5.0, 8.0, 
                   2.0, 5.0, 
                   7.0, 2.0};
  int row[5] = {0,2,4,6,8};
  int col[8] = {0,1,0,1,0,1,0,1};
  ctimer_t ct;
  
  ctimer_start(&ct);

  sparse_matrix_t *sm = malloc(sizeof(sparse_matrix_t));
  if (sm != NULL) {
    sm->n = 4;
    sm->nnz = 8;
    sm->rows = row;
    sm->cols = col;
    sm->vals = val;
  } else {
    return -1;
  }

  spmv(sm, inp, out);

  ctimer_stop(&ct);
  ctimer_measure(&ct);

  for(int i = 0; i < sm->n; i+=2) {
    printf("r/c = %02f/%02f\n",out[i], out[i+1]);
  }

  ctimer_print(ct, "spmv");

  free(sm);
  return 0;
}
