#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

timebasestruct_t start, finish;
int val = 3;
int w1, w2;
double time;

// Starts the timer
void timer_start() {
  read_real_time(&start, TIMEBASE_SZ);
}

// Ends and returns the time elapsed in milliseconds
double timer_end() {
  read_real_time(&finish, TIMEBASE_SZ);
  time_base_to_time(&start, TIMEBASE_SZ);
  time_base_to_time(&finish, TIMEBASE_SZ);
  w1 = finish.tb_high - start.tb_high;
  w2 = finish.tb_low - start.tb_low;
  if (w2 < 0) {
    w1--;
    w2 += 1000000000;
  }
  time = ((double) w2)/1000.0;
  if (w1 > 0)
    time += ((double) w1)*1000000.0;
  return time/1000;
}


void func1(int *p, int *o) {
  unsigned long long size = 31*1024ULL*1024ULL;
  // TODO: Parallelize the loop using OpenMP
  for (int i = 0 ; i < size ; i+=1) {
    o[i] = sqrt(p[i]);
  }
}

void func2(int *p, int *o) {
  unsigned long long size = 4*1024ULL*1024ULL;
  // TODO: Parallelize the loop using OpenMP
  for (int i = 0 ; i < size ; i+=1) {
    o[i] = sqrt(p[i]);
  }
}

int main (int argc, char **argv) {
  int *p, *o;
  unsigned long long size = 31*1024ULL*1024ULL;

  printf("Starting program for Assignment 3\n");

  // Memory allocation
  p = (int*)malloc(sizeof(int)*size);
  o = (int*)malloc(sizeof(int)*size);
  
  // Initializing the malloc-ed array
  for (unsigned long long i = 0 ; i < size ; i++) {
    p[i] = 1*rand();
  }

  // TODO: Add your time measuring code here. 
  func1(p, o);
  func2(p, o);
  
  // Free the malloc-ed array
  free(p);
  free(o);
  
  return 0;
}
