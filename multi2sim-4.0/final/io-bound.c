#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_LINE 1024

void read_file(const char* filename) {
  struct stat file_stat;
  int fd = open(filename, O_RDONLY);
  fstat(fd, &file_stat);
  int size = file_stat.st_size;
  int len = 0;

  //char* buffer = (char*) malloc( sizeof(char) * size);
  char buffer[MAX_LINE];
  while(len < size) {
    int st = read(fd, buffer, MAX_LINE);
    len += st;
  }
  /*
  len = read(fd, buffer, size);
  if (len != size ) {
    free(buffer);
    fprintf(stderr, "Problem when reading\n");
    return ;
  }
  */
}

void * read_thread(void *filename) {
  struct timeval start, end;
  gettimeofday(&start, NULL);
  read_file((const char* )filename);
  gettimeofday(&end, NULL);
  fprintf(stderr, "The running time of reading %s is %f\n", (const char*) filename, 1.0 * ((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec) ) / 1000000);
  return NULL;
}


int main(int argc, char** argv) {
  char* filenames[] = {"file1",
             "file2", 
             "file3",
             "file4"};

  int i = 0;
  int order[4] = {0};
  for(i =0; i< 4; i ++ ) {
    order[i] = atoi(argv[i+1]);
  }
  pthread_t thread_id[4];
  for(i = 0; i < 4; i ++ ) {
    printf("create a thread to read file %s\n", filenames[order[i]]);
    pthread_create(&thread_id[i], NULL, read_thread, (void*) filenames[order[i]]);
  }

  for(i = 0; i < 4; i ++ ) {
    pthread_join(thread_id[i], NULL);
  }

  return 0;
}
