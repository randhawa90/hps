#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>


#define NUM 8
#define L(i) 2*(i) + 1
#define R(i) 2 *(i) + 2
#define P(i) ((i) - 1) / 2



typedef struct {
  float dis;
  int f;
  int t;
}Node;

typedef  struct Minheap {
  Node data[NUM * NUM];
  int size;
}Minheap;

Minheap heap;

void shift_down(Minheap * heap, int i ) {
  int flag = 0;
  while( i < heap->size) {
    Node min = heap->data[i];
    if ( L(i) < heap->size) {
      if ( min.dis > heap->data[L(i)].dis) {
        min = heap->data[L(i)];
        flag = 1;
      }
    }
    if ( R(i) < heap->size) {
      if ( min.dis > heap->data[R(i)].dis) {
        min = heap->data[R(i)];
        flag = 2;
      }
    }

    if (flag == 0)
      return;
    int idx = flag == 1? L(i) : R(i);
    heap->data[idx] = heap->data[i];
    heap->data[i] = min;
    i = flag == 1? L(i) : R(i);
    flag = 0;
  }
}


void shift_up(Minheap* heap, int i ) {
  while( i != 0 ) {
    if(heap->data[i].dis < heap->data[P(i)].dis) {
      Node n = heap->data[P(i)];
      heap->data[P(i)] = heap->data[i];
      heap->data[i] = n;
      i = P(i);
    }
    else
      return;
  }
}

void create_heap(Minheap * heap, float cost[NUM], int len) {
  heap->size = len;
  int i ;
  for(i = 0 ; i < len; i ++ ) {
    heap->data[i].dis = cost[i];
    heap->data[i].f = 0;
    heap->data[i].t = i;
  }

  for(i = len / 2 - 1; i >= 0; i -- ) {
    shift_down(heap, i);
  }
}

void add_heap(Minheap * heap, Node n) {
  heap->data[heap->size] = n;
  heap->size ++;
  shift_up(heap, heap->size - 1);
}

Node extract_heap(Minheap* heap) {
  Node min = heap->data[0];
  int last = heap->size - 1;
  heap->data[0] = heap->data[last];
  heap->size --;
  shift_down(heap, 0);
  return min;
}

typedef struct City {
  int mark;
  int id;
  float x;
  float y;
  float z;
}City;


float distance(City* a, City* b) {
    return sqrt((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y) + (a->z - b->z) *(a->z - b->z) );
}


int load(char* filename, City* cities) {
  int len = 0;
  FILE* fin;
  if ( (fin = fopen(filename, "r") ) == NULL) {
    fprintf(stderr, "Cann't open file\n");
    exit(1);
  }

  while( (fscanf(fin, "%d %f %f %f", &cities[len].id, &cities[len].x, &cities[len].y, &cities[len].z) == 4)) {
    //printf("%d %d %d %d\n", cities[len].id, cities[len].x, cities[len].y, cities[len].z);
    len ++;
  }
  fclose(fin);
  return len;
}

void
compute_cost(City * cities, float cost[NUM][NUM], int len) {
  int i, j;
  City* a, *b;
  for(i = 0; i < len; i ++ ) {
    a = &cities[i];
    for(j = 0; j < len; j ++ ) {
      b = &cities[j];
      cost[i][j] = distance(a, b);
    }
  }
}

float tsp(float cost[NUM][NUM], City * cities, int len, int city_id[]) {
  float rst = 0;
  int i, j;
  city_id[0] = cities[0].id;

  for(i = 0; i < len; i ++ ) {
    float min = 10000000;
    int idx = -1;

    for(j = 0; j < len; j ++) {
      if (i != j && cities[j].mark != 1 && cost[i][j] < min ) {
        min = cost[i][j];
        idx = j;
      }
    }

    city_id[i + 1] = cities[idx].id;
    cities[idx].mark = 1;
    rst += min;
  }
  return rst;
}

typedef struct Graph {
  float cost[NUM][NUM];
  int mark[NUM];
  int parent[NUM];
}Graph;


void create_graph(Graph* g, float  cost[NUM][NUM]) {
  memset(g, 0, sizeof(Graph));
  int i, j;
  for(i = 0; i < NUM; i ++ ) {
    for(j = 0; j < NUM; j ++ ) {
      g->cost[i][j] = cost[i][j];
    }
  }
}

float compute_mst(Graph * g, Graph * mst) {
  float  total = 0;
  int num = 1;
  int f, t;
  f = 0;

  int i,j;
  create_heap(&heap, g->cost[f], NUM);
  g->parent[f] = f;
  g->mark[f] = 1;
  while(num < NUM) {
    Node n = extract_heap(&heap);
    while(n.t == n.f || g->mark[n.t] ) {
      n =  extract_heap(&heap);
    }
    f = n.f;
    t = n.t;
    total += n.dis;
    mst->cost[f][t] = n.dis;
    mst->cost[t][f] = n.dis;
    g->parent[t] = f;
    f = t;
    g->mark[f] = 1;
    for(j = 0; j < NUM; j ++) {
      if(j == f || g->mark[j]) continue;
      Node n = {g->cost[f][j], f, j};
      add_heap(&heap, n);
    }
    num++;
  }
  return total;
}


int compute_odd(Graph* mst, int odd_list[] ) {
  int len  = 0;
  int i;
  for(i = 0; i < NUM; i ++ ) {
    int num = 0;
    int j;
    for(j = 0; j < NUM; j ++ ) {
      if (mst->cost[i][j] > 0 ) {
        num ++;
      }
    }
    if (num  % 2 ) {
      odd_list[len ++ ] = i;
    }
  }
  return len;
}


void nn_mwm(float cost[NUM][NUM], int odd_list[], int len, Graph * mst, City * cities) {
  int i;
  int *mark = (int*) malloc (sizeof(int) * len);
  memset(mark, 0, sizeof(int) * len);

  float total = 0;

  /*
  for(i = 0; i < len; i ++) {
    printf("%d ", odd_list[i]);
  }
  printf("\n");
  */
  for(i = 0; i < len; i ++ ) {
    int f = odd_list[i];
    if (mark[i])
      continue;
    mark[i] = 1;
    int t, j, idx;
    int min = 100000000;
    for(j = 0; j < len; j ++ ) {
      if ( j == i || mark[j] == 1) continue;
      t = odd_list[j];
      if (min > cost[f][t]){
        min = cost[f][t];
        idx = j;
      }
    }
    t = odd_list[idx];
    mst->cost[f][t] = min;
    mst->cost[t][f] = min;
    mark[idx] = 1;
    //printf("match one pair (%d %d)\n", f, t);
    total += min;
  }

  printf("The minimal weighht perfect matching's length is %f\n", total);
  free(mark);
}


void greed_mwm(float cost[NUM][NUM], int odd_list[], int len, Graph* mst, City* cities) {
  City center = {0, 1001, 0, 0, 0};
  int i;
  int * mark = (int*) malloc( sizeof(int) * len);
  memset(mark, 0, sizeof(int) * len);
  int num = 0;
  float total = 0;
  int size = len;
  while( num < len/ 2) {
    center.x = center.y = center.z = 0;
    int time = 0;
    for(i = 0; i < len; i ++ ) {
      if(mark[i]) continue;
      int id = odd_list[i];
      center.x += cities[id].x;
      center.y += cities[id].y;
      center.z += cities[id].z;
      time ++;
    }
    assert(time == size);
    center.x /= size;
    center.y /= size;
    center.z /= size;
    //printf("%f %f %f\n", center.x , center.y, center.z);
    int start = 0;
    float dis = 0;
    float tmp =0;
    for(i = 0;i < len; i ++ ) {
      if(mark[i]) continue;
      tmp = distance(&center, &cities[odd_list[i]]);
      if( tmp > dis) {
        start = i;
        dis = tmp;
      }
    }
    mark[start] = 1;
    float min = FLT_MAX;
    int t, f = odd_list[start];
    int idx = 0;
    for(i = 0; i < len; i ++ ) {
      if ( i == start || mark[i] == 1) continue;
      t = odd_list[i];
      if (min > cost[f][t]){
        min = cost[f][t];
        idx = i;
      }
    }
    t = odd_list[idx];
    mst->cost[f][t] = min;
    mst->cost[t][f] = min;
    mark[idx] = 1;
    num ++;
    printf("%d %d\n", f, t);
    total += min;
    size -= 2;
  }
  printf("The cost of minimal match pairs is %f\n", total);
  free(mark);
}

void compute_mwm(float cost[NUM][NUM], int odd_list[], int len, Graph * mst, City * cities) {
  greed_mwm(cost, odd_list, len, mst, cities);
}

void walk_path(float cost[NUM][NUM], Graph* mst, int tmp_id[], int f, int num, float dis, float *pmin, int city_id[]) {
  mst->mark[f] = 1;
  tmp_id[num] = f;
  num ++;
  //printf("%d\n", num);
  if (num == NUM) {
    /*
    int start = tmp_id[0];
    int end = tmp_id[NUM-1];
    dis += cost[end][start];
    */
    printf("find a path\ttotal distance is %f\n", dis);
    if( *pmin > dis) {
      int i;
      for(i = 0 ;i < NUM; i ++ ) {
        printf("%d ", tmp_id[i]);
        city_id[i] = tmp_id[i];
        *pmin = dis;
      }
      printf("\n");
    }
    mst->mark[f] = 0;
    return;
  }
  int flag = 0;
  int i;
  for(i = 0; i < NUM; i ++ ) {
    if(mst->cost[f][i] > 0 && mst->mark[i] != 1 ){
      flag = 1;
      walk_path(cost, mst, tmp_id, i, num, dis + cost[f][i], pmin, city_id);
    }
  }
  mst->mark[f] = 0;
}

int pick_start(Graph* mst ) {
  int i;
  for(i = 0; i < NUM;i ++ ) {
    int num = 0;
    int j;
    for(j = 0; j < NUM; j++ ) {
      if (mst->cost[i][j] > 0) {
        num ++;
      }
    }
    if( num == 1) {
      return i;
    }
  }
  return -1;
}

void DFS(float cost[NUM][NUM], Graph* mst, int f, int tmp_id[], int num) {
  int i;
  tmp_id[num] = f;
  num ++;
  for(i = 0; i < NUM; i++ ) {
    if( mst->cost[f][i] > 0 ) {
      if( mst->mark[i] == 1) {
        int j = 0;
        for(; j < num -1; j ++ ) {
          if(tmp_id[j] == i) {
            int t = tmp_id[j + 1];
            if(mst->cost[i][t] > mst->cost[f][i]) {
              mst->cost[i][t] = 0;
              mst->cost[t][i] = 0;
            }else {
              mst->cost[f][i] = 0;
              mst->cost[i][f] = 0;
            }
            break;
          }
        }
      }
      else {
        DFS(cost, mst, i, tmp_id, num);
      }
    }
  }
}
float splice(float cost[NUM][NUM], Graph * mst, int city_id[]) {
  
}

float choose_path(float cost[NUM][NUM], Graph* mst, int city_id[]) {
  memset(mst->mark, 0, sizeof(int) * NUM);
  int f = 0 ; //pick_start(mst);
  /*
  if (f == -1 ) {
    fprintf(stderr, "shit\n");
    exit(-1);
  }
  */
  float min = FLT_MAX;
  int tmp_id[NUM];
  walk_path(cost, mst, tmp_id ,f , 0, 0, &min, city_id);
  return min;
}

float tsp_mst(Graph *g, float cost[NUM][NUM], City* cities, int len, int city_id[] ) {
  float total = 0;
  Graph mst;
  memset(&mst, 0, sizeof(mst));
  create_graph(g, cost);
  total = compute_mst(g, &mst);
  printf("The cost of mst is %f\n", total);
  /*
  int i, j;
  for(i = 0;i < NUM ; i++ ) {
    for(j = 0 ; j < NUM; j ++ ) {
      printf("%6.2f ", cost[i][j]);
    }
    printf("\n");
  }

  printf("\n");

  for(i = 0;i < NUM ; i++ ) {
    for(j = 0 ; j < NUM; j ++ ) {
      printf("%6.2f ", mst.cost[i][j]);
    }
    printf("\n");
  }

  */

  int odd_list[NUM];
  int size = compute_odd(&mst, odd_list);
  printf("There are %d odd-degree vertices\n", size);
  compute_mwm(cost, odd_list, size, &mst, cities);
  int i, j;
  /*
  for(i = 0;i < NUM ; i++ ) {
    for(j = 0 ; j < NUM; j ++ ) {
      printf("%6.2f ", mst.cost[i][j]);
    }
    printf("\n");
  }
  */

  total = choose_path(cost, &mst, city_id);
  return total;
}

int main() {
  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);
  char* filename = "mytest";
  int len;
  City cities[NUM];
  memset(&cities, 0, sizeof(City) * NUM);
  len = load(filename, cities);

  float cost[NUM][NUM]  = {0.0f};
  int city_id[NUM + 1] = {0};

  compute_cost(cities, cost, len);

  //float total_cost = tsp(cost, cities, len, city_id);
  Graph *g = (Graph*) malloc (sizeof (Graph) );
  float total_cost = tsp_mst(g, cost, cities, len, city_id);
  free(g);
  gettimeofday(&tv2, NULL);
  printf ("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));
  int i = 0;
  for(; i <= len; i ++ ){
    printf("%d ", city_id[i]);
  }
  printf("\n%f\n", total_cost);
  /*
  float cost[NUM][NUM] = { 1.2 , 2.1, 3.2, 5.2, 1.3, 4.2, 3.5, 4.1, 8.3, 5.5};
  Minheap heap;
  create_heap(&heap, cost[0], NUM);
  Node n = {1.0, 3, 4};
  add_heap(&heap, n);
  n = extract_heap(&heap);
  */
  return 0;
}
