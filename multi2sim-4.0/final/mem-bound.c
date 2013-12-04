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
#define BIGFILE "bigfile"
#define SMALLFILE "smallfile"

#define BIG_COMPUTATION_ID 0
#define SMALL_COMPUTATION_ID 1
#define BIG_READ_ID 2
#define SMALL_READ_ID 3


pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
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
  struct timeval start, end;
  gettimeofday(&start, NULL);
  matrixmult(MBIG, bigA, bigB, bigC);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of thread %d is %f\n", BIG_COMPUTATION_ID, 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}


void* matrixmult_small(void * a) {
  struct timeval start, end;
  gettimeofday(&start, NULL);
  matrixmult(MSAM, smallA, smallB, smallC);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of thread %d is %f\n", SMALL_COMPUTATION_ID, 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}


void read_file(const char* filename) {
  struct stat file_stat;
  int fd = open(filename, O_RDONLY);
  fstat(fd, &file_stat);
  int size = file_stat.st_size;
  int len = 0;
  
  char* buffer = (char*) malloc( sizeof(char) * size);
  len = read(fd, buffer, size);
  if (len != size ) {
    free(buffer);
    fprintf(stderr, "Problem when reading\n");
    return ;
  }
  
  free(buffer);
}

void* read_bigfile(void*a) {
  struct timeval start, end;
  gettimeofday(&start, NULL);
  read_file(BIGFILE);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of thread %d is %f\n", BIG_READ_ID, 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}

void * read_smallfile(void*a) {
  struct timeval start, end;
  gettimeofday(&start, NULL);
  read_file(SMALLFILE);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of thread %d is %f\n", SMALL_READ_ID, 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}


int main() {
  pthread_t big;
  pthread_t small;

  pthread_t bigread;
  pthread_t smallread;


  pthread_create(&big, NULL, matrixmult_big, NULL);
  pthread_create(&small, NULL, matrixmult_small, NULL);

  pthread_create(&bigread, NULL, read_bigfile, NULL);
  pthread_create(&smallread, NULL, read_smallfile, NULL);

  pthread_join(small, NULL);
  pthread_join(big, NULL);
  pthread_join(smallread, NULL);
  pthread_join(bigread, NULL);
  return 0;
}
