#include <stdio.h>
#include <sys/time.h>

double N = 30;

int mintit(int denom[], int len) {
  int i;
  int coins[100];
  int s = 0;

  for(i = 0; i < 100; i ++ ) {
    coins[i] = 100;
  }
  coins[0] = 0;

  for (i = 0; i < len; i ++) {
    coins[denom[i]] = 1;
  }

  for (i = 1; i < 100; i ++) {
    int min = coins[i];
    int j;
    int tmp;
    for(j = 0; j < len; j ++) {
      if(i < denom[j]) {
        break;
      }
      else{
        tmp = coins[i - denom[j] ] + 1;
        if (tmp < min) {
          min = tmp;
          coins[i] = min;
        }
      }
    }
    s += coins[i];
  }

  return s;
}


int mintit_with_scoreescape(int denom[], int len, int score) {
  int i;
  int j;
  int coins[100];
  int s = 0;
  int min;
  int tmp;

  for(i = 0; i < 100; i ++ ) {
    coins[i] = 100;
  }
  coins[0] = 0;

  for (i = 0; i < len; i ++) {
    coins[denom[i]] = 1;
  }

  for (i = 1; i < 100; i ++) {
    min = coins[i];
    for(j = 0; j < len; j ++) {
      if(i < denom[j]) {
        break;
      }
      else{
        tmp = coins[i - denom[j] ] + 1;
        if (tmp < min) {
          min = tmp;
          coins[i] = min;
        }
      }
    }
    s += coins[i];
    if (s > score){
      return s;
    }
  }
  return s;
}


int mintit_with_scoreescape_with_initializecoins(int denom[], int len, int score, int coins[], int initialized) {
  int i;
  int j;
  int s = 0;
  if( initialized == 1) {
    for(i = 0; i < denom[len-1] - 1; i ++) {
      s += coins[i];
      if ( s > score) {
        return s;
      }
    }

    i = denom[len-1] - 1;
  }
  else {
    coins[0] = 0;
    i = 1;
  }
  for(; i <  100; i ++ ) {
    int min = 100;
    for(j = 0; j < len; j ++) {
      if(i == denom[j]) { coins[i] = 1;break;}
      if(i - denom[j] < 0)
        break;
      int tmp = coins[i - denom[j]] + 1;
      if( tmp < min) {
        min = tmp;
        coins[i] = min;
      }
    }
    s += coins[i];

    if ( s > score ) {
      return s;
    }
  }
  return s;
}
int mintit_with_scoreescape_with_initializecoins_with_fullscorereturn(int denom[], int len, int score, int coins[], int initialized) {
  int i;
  int j;
  int s = 0;
  if( initialized == 1) {
    for(i = 0; i < denom[len-1] - 1; i ++) {
      s += coins[i];
      if ( s > score) {
        return s + (100 - i - 1) * 2;
      }
    }

    i = denom[len-1] - 1;
  }
  else {
    coins[0] = 0;
    i = 1;
  }
  for(; i <  100; i ++ ) {
    int min = 100;
    for(j = 0; j < len; j ++) {
      if(i == denom[j]) { coins[i] = 1;break;}
      if(i - denom[j] < 0)
        break;
      int tmp = coins[i - denom[j]] + 1;
      if( tmp < min) {
        min = tmp;
        coins[i] = min;
      }
    }
    s += coins[i];

    if ( s > score ) {
      return s + (100 -i -1) * 2;
    }
  }
  return s;
}

void output(int coins[], int len){
  int i;
  for(i = 0; i < len; i ++ ) {
    printf("%d: %d\n", i, coins[i]);
  }
}


int mintit2(int denom[], int len){
  int i, j;
  double s = 0;
  int coins[51] = {0};
  for(i = 1; i <= 50; i ++ ) {
    coins[i] = -1;
  }

  for (i = 1; i <= 50; i ++ ) {
    int min = 100;
    int tmp;
    for (j = 0; j < len; j ++ ) {
      if (i == denom[j] ) { coins[i] = 1; break; }
      if (i > denom[j] ) {
        tmp = coins[i - denom[j] ] + 1;
        if(tmp < min) {
          min = tmp;
          coins[i] = min;
        }
      }

      if ( i + denom[j] > 50 && 100 - i - denom[j] < i) {
        tmp = coins[100 - i - denom[j]] +1;
        if (tmp < min) {
          min = tmp;
          coins[i] = min;
        }
      }
    }
  }

  for(i = 50; i >= 1; i-- ) {
    if(coins[i] == 1 || coins[i] == 2)
      goto EXIT;
    int min = coins[i];
    int tmp;
    for(j = 0; j < len; j ++ ) {
      /*
      if( i > denom[j] ) {
        tmp = coins[i - denom[j] ] + 1;
        if(tmp < min) {
          min = tmp;
          coins[i] = min;
        }
      }
      */
      int idx = i + denom[j];
      if (i + denom[j] > 50 )
        idx = 100 - idx;
      tmp = coins[idx] + 1;
      if (tmp < min) {
        min = tmp;
        coins[i] = min;
      }
    }
EXIT:
    if ( i % 5 == 0) {
    //printf("%d:%.1f\n", i,  1.0 *coins[i]);
      s += N * coins[i];
    }
    else{
      s += coins[i];
    }
  }
  for(i = 1; i < 50; i ++ ) {
    if ( i % 5 == 0) {
      s += N * coins[i];
    }
    else{
      s += coins[i];
    }
    //printf("%d:%.1f\n", i,  1.0 *coins[i]);
  }
  //printf("%d:%.1f\n", 50,  1.0 *coins[50]);
  return s;
}
int  main(int argc, char** argv) {
  /*
  int denom[5] = {1};
  int score = 100000;
  int tmp = 0;
  int end = 67;
  int i, j, m, n;
  int final[5];

  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);
  int coins[100] = {0, 1, 1, 1, 1, 1};
  for(i = 2; i < end - 3; i ++) {
    denom[1] = i;
    for(j = i + 1; j < end - 2; j ++){
      denom[2] = j;
      for(m = j + 1; m < end - 1; m ++ ) {
        denom[3] = m;
        int inner_score = 10000;
        int icount = 0;
        int initialized = 0;
        for(n = m + 1; n < end; n ++ ) {
          int k;
          denom[4] = n;
          tmp = mintit_with_scoreescape_with_initializecoins_with_fullscorereturn(denom, 5, score, coins, initialized);
          if (tmp < score) {
            score = tmp;
            for(k = 0; k < 5; k ++ )
              final[k] = denom[k];
          }
          initialized = 1;
          if (tmp < inner_score) {
            inner_score = tmp;
          }
          else {
            icount ++;
          }
          if (icount == 10){
            break;
          }
        }
      }
    }
  }
  printf("%d\n", score);
  for(i = 0; i < 5; i ++ ) {
    printf("%d ", final[i]);
  }
  printf("\n");

  gettimeofday(&tv2, NULL);
  printf ("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));
  */
  int denom[5] = {1};
  double score = 100000;
  double tmp = 0;
  int end = 51;
  int i, j, m, n;
  int final[5];

  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);
  int coins[100] = {0, 1, 1, 1, 1, 1};
  for(i = 2; i < end - 3; i ++) {
    denom[1] = i;
    for(j = i + 1; j < end - 2; j ++){
      denom[2] = j;
      for(m = j + 1; m < end - 1; m ++ ) {
        denom[3] = m;
        for(n = m + 1; n < end; n ++ ) {
          int k;
          denom[4] = n;
          tmp = mintit2(denom, 5);
          if (tmp < score) {
            score = tmp;
            for(k = 0; k < 5; k ++ )
              final[k] = denom[k];
          }
        }
      }
    }
  }
  printf("%f\n", N);
  printf("%lf\n", score);
  for(i = 0; i < 5; i ++ ) {
    printf("%d ", final[i]);
  }
  printf("\n");

  gettimeofday(&tv2, NULL);
  printf ("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));

  /*
  int denom[5] = {1, 4, 6, 19, 36};
  int score = mintit2(denom, 5);
  printf("%d\n", score);
  */

  return 0;
}
