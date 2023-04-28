/*
  gcc prog3.c -O0 -o prog3 -lm -fopenmp
  
  ./prog3
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


void func1(int *p, int *o) 
{
  unsigned long long size = 31*1024ULL*1024ULL;
  #pragma omp parallel for
  for (int i = 0 ; i < size ; i+=1) 
  {
    o[i] = sqrt(p[i]);
  }
}

void func2(int *p, int *o) 
{
  unsigned long long size = 4*1024ULL*1024ULL;
  #pragma omp parallel for
  for (int i = 0 ; i < size ; i+=1) 
  {
    o[i] = sqrt(p[i]);
  }
}

int main (int argc, char **argv) {
  int *p, *o;
  unsigned long long size = 31*1024ULL*1024ULL;

  int num_threads[] = {1, 2, 4, 8, 16, 28};
  int size_num_threads = 6;
  
  double exec_time[size_num_threads];

  double speed_up[size_num_threads];

  int RUNS = 10;

  double sum = 0.0;


  printf("Starting program for Assignment 3\n");

  // Memory allocation
  p = (int*)malloc(sizeof(int)*size);
  o = (int*)malloc(sizeof(int)*size);
  
  // Initializing the malloc-ed array
  for (unsigned long long i = 0 ; i < size ; i++) 
  {
    p[i] = 1*rand();
  }

  

  /* Calculate Execution time for func 1  */
  for (int i=0; i< size_num_threads;i++)
  { 
    omp_set_num_threads(num_threads[i]);
    
    timer_start();    
    for (int j=0; j< RUNS; j++)
    {
      func1(p,o);
    }
    sum = timer_end();
    exec_time[i] = sum/RUNS;
  }

/* Calculate speed-up for func 1*/
  for (int i=1; i< size_num_threads;i++)
  {
    speed_up[i] = exec_time[0] / exec_time[i];
  }

  /* Print speed_up for func 1*/
  printf("Part 1.a: "); 
  for(int i=1; i< size_num_threads;i++)
  {
   
    printf("%dT:%.2f, ", num_threads[i],speed_up[i]);
  }
  printf("\n");
  
  
  /* Calculate Execution time for func 2  */
  for (int i=0; i< size_num_threads;i++)
  {
    omp_set_num_threads(num_threads[i]);
    timer_start();
    for (int j=0; j< RUNS; j++)
    {
      func2(p,o);
    }
    sum = timer_end();   
    exec_time[i] = sum/RUNS;
  }

/* Calculate speed-up for func 2*/
  for (int i=1; i< size_num_threads;i++)
  {
    speed_up[i] = exec_time[0] / exec_time[i];
  }

  /* Print speed_up for func 2*/
  printf("Part 1.b: "); 
   for(int i=1; i< size_num_threads;i++)
  {
    printf("%dT:%.2f, ", num_threads[i],speed_up[i]);
  }
  printf("\n");
  
  printf("Part 1.c: We observed that func1() generally acheived higher speed-up than func2(). The loop size of func1() is more than the loop size of func2(). Thus, the loop in func1() runs for a longer period of time, meaning the overhead of starting up each thread will be a smaller portion of the total execution time. For smaller thread counts, the effect of overhead is minimal and there is little difference between each function's speed-up. However, as thread count increases, the overhead of creating and managing threads also increases. As previously discussed, funct1()'s speed-up is less impacted by overhead, leading to a divergence in the speed-ups for func1() and func2() around 8-16 threads. However, the overhead of 28 threads is a detriment to both functions, with speedup decreasing for func1() from 16 threads and func2() exhibiting a similar speedup to func1(). Thus, at mid-range thread counts func1() acheives higher speed-up than funct2(), while they are roughly equivalant for the lowest and highest thread counts.\n"); 
  
  // Free the malloc-ed array
  free(p);
  free(o);
  
  return 0;
}
