#include <stdio.h>
#include <pthread.h>

#define MBIG 33
#define MSAM 8

int bigA[MBIG][MBIG] = {0};
int bigB[MBIG][MBIG] = {0};
int bigC[MBIG][MBIG];


int smallA[MSAM][MSAM] = {0};
int smallB[MSAM][MSAM] = {0};
int smallC[MSAM][MSAM];

void matrixmult(int M, int A[M][M], int B[M][M], int C[M][M]) {
  int i, j, k;
  for(i = 0; i < M; i ++ ) {
    for(j = 0; j < M; j ++ ) {
      int sum = 0;
      for(k =0; k < M ; k ++ ) {
        sum += A[i][k] * B[k][j];
      }
      C[i][j] = sum;
    }
  }
}

void* matrixmult_big(void * a ) {
  matrixmult(MBIG, bigA, bigB, bigC);
  return NULL;
}


void* matrixmult_small(void * a) {
  matrixmult(MSAM, smallA, smallB, smallC);
  return NULL;
}


int main() {
  pthread_t big;
  pthread_t small;


  pthread_create(&big, NULL, matrixmult_big, NULL);
  pthread_create(&small, NULL, matrixmult_small, NULL);

  pthread_join(big, NULL);
  pthread_join(small, NULL);
  return 0;
}
