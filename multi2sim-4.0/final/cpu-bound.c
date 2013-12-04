#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>


float random_gen(float from, float to) {
  int r = rand();
  float fr = (float) r / (float) INT_MAX;
  fr = fr * (to - from);
  fr = fr + from;
  return fr;
}


int circle_counts[4];
int npoints[4] = {100, 500, 1000, 2000};

void* pi(void *a) {
  struct timeval start, end;
  gettimeofday(&start, NULL);
  int npoint = npoints[(int)a];

  long circle_count = 0;
  int i;
  for(i = 0; i < npoint; i ++){
    float x = random_gen(0, 1);
    float y = random_gen(0, 1);
    float dis = sqrt(x * x + y * y);
    if ( dis <= 1) {
      circle_count ++;
    }
  }
  circle_counts[(int)a] = circle_count;
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of the computation thread %d is %f\n", (int)a, 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}

int main() {
  pthread_t pid[4];
  int num_points = 0;
  int num_circle_count = 0;
  int i;
  for(i = 0; i < 4; i ++ )  {
    pthread_create(&pid[i], NULL, pi, (void*)i);
    num_points += npoints[i];
  }
  for(i = 0; i < 4; i ++ ) {
    pthread_join(pid[i], NULL);
    num_circle_count += circle_counts[i];
  }

  printf("The pi is %f\n", 4.0 *num_circle_count / num_points);

  return 0;
}
