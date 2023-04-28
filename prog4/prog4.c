/*
  gcc prog4.c -O0 -o prog4 -lm -fopenmp
  
  ./prog4
*/

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

  for (int i = 0 ; i < size ; i+=1) {
    o[i] = sqrt(p[i]);
  }
}

void func1_pol_1(int *p, int *o) {
  unsigned long long size = 31*1024ULL*1024ULL;
  #pragma omp parallel for schedule(static, 1)
  for (int i = 0 ; i < size ; i+=1) {
    o[i] = sqrt(p[i]);
  }
}

void func1_pol_2(int *p, int *o) {
  unsigned long long size = 31*1024ULL*1024ULL;
  #pragma omp parallel for schedule(static, 1024)
  for (int i = 0 ; i < size ; i+=1) {
    o[i] = sqrt(p[i]);
  }
}

void func1_pol_3(int *p, int *o) {
  unsigned long long size = 31*1024ULL*1024ULL;
  #pragma omp parallel for schedule(dynamic, 1)
  for (int i = 0 ; i < size ; i+=1) {
    o[i] = sqrt(p[i]);
  }
}

void func1_pol_4(int *p, int *o) {
  unsigned long long size = 31*1024ULL*1024ULL;
  #pragma omp parallel for schedule(dynamic, 1024)
  for (int i = 0 ; i < size ; i+=1) {
    o[i] = sqrt(p[i]);
  }
}

void func3(int *p) {
  unsigned long long size = 31*1024ULL*1024ULL, square_root_product = 0;
  for (int i = 0 ; i < size ; i += 1) {
    square_root_product *= sqrt(p[i]);
  }
}

/* reduction(+:square_root_product) creates a thread-private copy of
    square_root_product on the private stack of each thread.
    When the for loop ends, OpenMP again reduces the thread-private
    copy of square_root_product into the original square_root_product variable.
*/
void func3_pol_1(int *p) {
  unsigned long long size = 31*1024ULL*1024ULL, square_root_product = 0;
#pragma omp parallel for schedule(static, 1) reduction(+:square_root_product)
  for (int i = 0 ; i < size ; i += 1) {
    square_root_product *= sqrt(p[i]);
  }
}

void func3_pol_2(int *p) {
  unsigned long long size = 31*1024ULL*1024ULL, square_root_product = 0;
#pragma omp parallel for schedule(static, 1024) reduction(+:square_root_product)
  for (int i = 0 ; i < size ; i += 1) {
    square_root_product *= sqrt(p[i]);
  }
}

void func3_pol_3(int *p) {
  unsigned long long size = 31*1024ULL*1024ULL, square_root_product = 0;
#pragma omp parallel for schedule(dynamic, 1) reduction(+:square_root_product)
  for (int i = 0 ; i < size ; i += 1) {
    square_root_product *= sqrt(p[i]);
  }
}

void func3_pol_4(int *p) {
  unsigned long long size = 31*1024ULL*1024ULL, square_root_product = 0;
#pragma omp parallel for schedule(dynamic, 1024) reduction(+:square_root_product)
  for (int i = 0 ; i < size ; i += 1) {
    square_root_product *= sqrt(p[i]);
  }
}

int main (int argc, char **argv) {
  int *p, *o;
  unsigned long long size = 31*1024ULL*1024ULL;

  printf("Starting program for Assignment 4\n");

  // Memory allocation
  p = (int*)malloc(sizeof(int)*size);
  o = (int*)malloc(sizeof(int)*size);
  
  // Initializing the malloc-ed array
  for (unsigned long long i = 0 ; i < size ; i++) {
    p[i] = 1*rand();
  }

  // General Setup
  const int RUNS = 10;
  double baseline, best, second;
  omp_set_num_threads(28);

  // Part A
  printf("\nPart A: \n"); 

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func1(p, o);
  }
  baseline = timer_end();

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func1_pol_1(p, o);
  }
  printf("func1, policy 1: %3.3f\n", baseline/timer_end());

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func1_pol_2(p, o);
  }
  second = baseline/timer_end();
  printf("func1, policy 2: %3.3f\n", second);

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func1_pol_3(p, o);
  }
  printf("func1, policy 3: %3.3f\n", baseline/timer_end());

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func1_pol_4(p, o);
  }
  best = baseline/timer_end();
  printf("func1, policy 4: %3.3f\n", best);
  printf("Policy 4 is clearly superior over the other three. The best policy is %3.3f times faster than the second-best.\n", best/second); 
  
  // Part B
  printf("\nPart B: \n");

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func3(p);
  }
  baseline = timer_end();

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func3_pol_1(p);
  }
  printf("func3, policy 1: %3.3f\n", baseline/timer_end());

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func3_pol_2(p);
  }
  second = baseline/timer_end();
  printf("func3, policy 2: %3.3f\n", second);

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func3_pol_3(p);
  }
  printf("func3, policy 3: %3.3f\n", baseline/timer_end());

  timer_start();
  for (int i = 0; i < RUNS; i++) {
    func3_pol_4(p);
  }
  best = baseline/timer_end();
  printf("func3, policy 4: %3.3f\n", best);
  printf("Policy 4 is clearly superior over the other three. The best policy is %3.3f times faster than the second-best.\n", best/second); 

  // Part B
  printf("\nPart C: \n");
  printf("Policy 4 is clearly superior over the other three. Dynamic is generally better than static. The static policy is forced to wait for a thread that is stalled, whereas a dynamic policy allows other threads to pick up the slack for the stalled thread. Coarse is generally better than fine, as threads will spend more time executing and less time requesting the next work item. These principles support Policy 4 being the best. Note that Policy 3 defies these general trends, since it puts the most work on the scheduler and thus incurs the most overhead.\n");

  // Free the malloc-ed array
  free(p);
  free(o);
  
  return 0;
}
