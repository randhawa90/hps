#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MBIG 33
#define MSAM 8
#define MMID 15


pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
int bigA[MBIG][MBIG] = {0};
int bigB[MBIG][MBIG] = {0};
int bigC[MBIG][MBIG];


int smallA[MSAM][MSAM] = {0};
int smallB[MSAM][MSAM] = {0};
int smallC[MSAM][MSAM];

int middleA[MMID][MMID] = {0};
int middleB[MMID][MMID] = {0};
int middleC[MMID][MMID];

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
  struct timeval start, end;
  gettimeofday(&start, NULL);
  matrixmult(MBIG, bigA, bigB, bigC);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of big thread is %f\n", 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}


void* matrixmult_small(void * a) {
  struct timeval start, end;
  gettimeofday(&start, NULL);
  matrixmult(MSAM, smallA, smallB, smallC);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of small thread is %f\n",  1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}


void * matrixmult_mid(void* a) {
  struct timeval start, end;
  gettimeofday(&start, NULL);
  matrixmult(MMID, middleA, middleB, middleC);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of middle, thread is %f\n", 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}
typedef void * (*func) (void*a);
int main(int argc, char** argv) {
  int order[4];
  int i;
  func funcs[4] = { matrixmult_big, matrixmult_mid, matrixmult_mid, matrixmult_small};
  for(i = 0; i< 4; i ++ ) {
    order[i] = atoi(argv[i + 1]);
  }
  pthread_t pid[4];
  for(i = 0; i< 4; i ++ ) {
    pthread_create(&pid[i], NULL, funcs[order[i]],NULL);
  }
  for(i = 0; i < 4; i ++ ) {
    pthread_join(pid[i], NULL);
  }
  return 0;
}
